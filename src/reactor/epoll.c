#include "epoll.h"
#include "log.h"

static int setnonblocking(int fd)
{
    int old_option;
    int new_option;

    old_option = fcntl(fd, F_GETFL);
    new_option = old_option | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, new_option) < 0) {
        tswWarn("fcntl error: %s", strerror(errno));
        return TSW_ERR;
    }

    return TSW_OK;
}

static int tswReactorEpoll_add(tswReactor *reactor, int fd, int tsw_event_type, int (*tswReactor_handler)(tswReactor *reactor, tswEvent *tswev))
{
    if (tsw_event_type == TSW_EVENT_READ) {
        if (epoll_add(reactor, fd, EPOLLIN | EPOLLET, tswReactor_handler) < 0) {
            tswWarn("epoll_add error: %s", strerror(errno));
            return TSW_ERR;
        }
    }
    if (tsw_event_type == TSW_EVENT_WRITE) {
        if (epoll_add(reactor, fd, EPOLLOUT | EPOLLET, tswReactor_handler) < 0) {
            tswWarn("epoll_add error: %s", strerror(errno));
            return TSW_ERR;
        }
    }
    reactor->event_num++;

    return TSW_OK;
}

static int tswReactorEpoll_set(tswReactor *reactor, int fd, int event_type)
{
    tswReactorEpoll *reactor_epoll_object = reactor->object;

    if (event_type == TSW_EVENT_READ) {
        epoll_event_set(reactor_epoll_object->epfd, fd, EPOLLIN | EPOLLET);
    }
    if (event_type == TSW_EVENT_WRITE) {
        epoll_event_set(reactor_epoll_object->epfd, fd, EPOLLOUT | EPOLLET);
    }

    return TSW_OK;
}

static int tswReactorEpoll_del(tswReactor *reactor, int fd)
{
    tswReactorEpoll *reactor_epoll_object = reactor->object;

    if (epoll_ctl(reactor_epoll_object->epfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
        tswWarn("epoll_ctl error: %s", strerror(errno));
        return TSW_ERR;
    }
    return TSW_OK;
}

static int tswReactorEpoll_wait(tswReactor *reactor)
{
    int nfds;
    int epollfd;
    tswReactorEpoll *reactor_epoll_object;
    struct epoll_event *events;
    int max_event_num;

    reactor_epoll_object = reactor->object;
    epollfd = reactor_epoll_object->epfd;
    events = reactor_epoll_object->events;
    max_event_num = reactor->max_event_num;

    if (events == NULL) {
        tswWarn("malloc error");
        return TSW_ERR;
    }

    nfds = epoll_wait(epollfd, events, max_event_num, -1);

    return nfds;
}

static int tswReactorEpoll_free(tswReactor *reactor)
{
    tswReactorEpoll *reactor_epoll_object = reactor->object;
    close(reactor_epoll_object->epfd);
    free(reactor_epoll_object->events);
    free(reactor_epoll_object);
}

int tswReactorEpoll_create(tswReactor *reactor, int max_event_num)
{
    tswReactorEpoll *reactor_epoll_object;

    reactor_epoll_object = malloc(sizeof(tswReactorEpoll));
    if (reactor_epoll_object == NULL) {
        tswWarn("malloc error");
        return TSW_ERR;
    }

    reactor_epoll_object->epfd = epoll_create(512);
    if (reactor_epoll_object->epfd < 0) {
        tswWarn("epoll_create failed. Error: %s[%d]", strerror(errno), errno);
        free(reactor_epoll_object);
        return TSW_ERR;
    }
    reactor_epoll_object->events = malloc(sizeof(struct epoll_event) * max_event_num);
    if (reactor_epoll_object->events == NULL) {
        tswWarn("malloc error");
        free(reactor_epoll_object);
        return TSW_ERR;
    }

    reactor->object = reactor_epoll_object;
    reactor->event_num = 0;
    reactor->max_event_num = max_event_num;

    reactor->add = tswReactorEpoll_add;
    reactor->set = tswReactorEpoll_set;
    reactor->del = tswReactorEpoll_del;
    reactor->wait = tswReactorEpoll_wait;
    reactor->free = tswReactorEpoll_free;

    return TSW_OK;
}

int epoll_add(tswReactor *reactor, int fd, int event_type, int (*tswReactor_handler)(tswReactor *reactor, tswEvent *tswev))
{
    struct epoll_event e;
    tswEvent *tswev;
    tswReactorEpoll *reactor_epoll_object;

    reactor_epoll_object = reactor->object;
    tswev = (tswEvent *)malloc(sizeof(tswEvent));

    // setnonblocking(fd);
    tswev->fd = fd;
    tswev->event = event_type;
    if (reactor->setHandler(tswev, tswReactor_handler) < 0) {
        return TSW_ERR;
    }
    e.data.ptr = tswev;
    e.events = event_type;

    if (epoll_ctl(reactor_epoll_object->epfd, EPOLL_CTL_ADD, fd, &e) < 0) {
        tswWarn("epoll_ctl error: %s", strerror(errno));
        return TSW_ERR;
    }

    return TSW_OK;
}

/*
 * Register a descriptor as writable
*/
int epoll_set_output(int epollfd, int fd)
{
    struct epoll_event e;

    e.data.fd = fd;
    e.events = EPOLLOUT | EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &e) < 0) {
        tswWarn("epoll_ctl error: %s", strerror(errno));
        return TSW_ERR;
    }

    return TSW_OK;
}

/*
 * Modify the listen event
*/
int epoll_event_set(int epollfd, int fd, int event_type)
{
    struct epoll_event e;

    e.data.fd = fd;
    e.events = event_type;
    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &e) < 0) {
        tswWarn("epoll_ctl error: %s", strerror(errno));
        return TSW_ERR;
    }

    return TSW_OK;
}