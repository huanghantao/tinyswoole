#include "../../include/epoll.h"

static int setnonblocking(int fd)
{
    int old_option;
    int new_option;

    old_option = fcntl(fd, F_GETFL);
    new_option = old_option | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, new_option) < 0) {
        perror("fcntl error: ");
        return TSW_ERR;
    }

    return TSW_OK;
}

int epoll_add(int epollfd, int fd, int event_type, int flag)
{
	struct epoll_event e;

	if (flag) {
        setnonblocking(fd);
    }

	e.data.fd = fd;
	e.events = event_type;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &e) < 0) {
        perror("epoll_ctl error: ");
        return TSW_ERR;
    }

	return TSW_OK;
}

int epoll_del(int epollfd, int fd)
{
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
        perror("epoll_ctl error: ");
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
        perror("epoll_ctl error: ");
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
        perror("epoll_ctl error: ");
        return TSW_ERR;
    }

    return TSW_OK;
}