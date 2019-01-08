#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include "tswoole_config.h"
#include "../../include/server.h"
#include "../../include/epoll.h"
#include "../../include/process_pool.h"
#include "../../include/worker.h"

tswServer *tswServer_new(void)
{
    tswServer *serv;

    serv = (tswServer *)malloc(sizeof(tswServer));
    if (serv == NULL) {
        return NULL;
    }
    serv->onStart = NULL;
    serv->onConnect = NULL;
    serv->onReceive = NULL;
    serv->onClose = NULL;
    serv->onReactorStart = NULL;

    serv->connection_list = (tswConnection *)malloc(sizeof(tswConnection) * TSW_CONNECTION_LIST_SIZE);
    if (serv->connection_list == NULL) {
        tswWarn("%s", "malloc error");
        return NULL;
    }

    serv->session_list = (tswSession *)malloc(sizeof(tswSession) * TSW_SESSION_LIST_SIZE);
    if (serv->session_list == NULL) {
        tswWarn("%s", "malloc error");
        return NULL;
    }

    serv->status = (tswServerStatus *)malloc(sizeof(tswServerStatus));
    if (serv->status == NULL) {
        tswWarn("%s", "malloc error");
        return NULL;
    }
    serv->status->accept_count = 0;

    return serv;
}

/*
 * Create reactor threads
*/
static int tswServer_start_proxy(tswServer *serv)
{
    tswReactor *main_reactor = malloc(sizeof(tswReactor));
    if (main_reactor == NULL) {
        tswWarn("%s", "malloc error");
        return TSW_ERR;
    }

    if (tswReactor_create(main_reactor, MAXEVENTS) < 0) {
        tswWarn("%s", "tswReactor_create error");
        return TSW_ERR;
    }

    if (tswReactorThread_create(serv) < 0) {
        tswWarn("%s", "tswReactorThread_create error");
        return TSW_ERR;
    }

    if (tswReactorThread_start(serv) < 0) {
        tswWarn("%s", "tswReactorThread_start error");
        return TSW_ERR;
    }

    if (listen(serv->serv_sock, LISTENQ) < 0) {
        tswWarn("%s", strerror(errno));
    }
    if (serv->onStart != NULL) {
        serv->onStart(serv);
    }

    if (main_reactor->add(main_reactor, serv->serv_sock, TSW_EVENT_READ, tswServer_master_onAccept) < 0) {
        tswWarn("%s", "reactor add error");
        return TSW_ERR;
    }

    for (;;) {
        int nfds;

        nfds = main_reactor->wait(main_reactor);
        if (nfds < 0) {
            tswWarn("%s", "master thread epoll wait error");
            return TSW_ERR;
        }
        
        for (int i = 0; i < nfds; i++) {
            tswReactorThread *tsw_reactor_thread;
            tswReactorEpoll *reactor_epoll_object = main_reactor->object;

            tswEvent *tswev = (tswEvent *)reactor_epoll_object->events[i].data.ptr;
            tswDebug("%s", "master thread handler the event");
            if (tswev->event_handler(main_reactor, tswev) < 0) {
                tswWarn("%s", "event_handler error");
                continue;
            }
        }
    }

    close(serv->serv_sock);

    return TSW_OK;
}

int tswServer_start(tswServer *serv)
{
    tswProcessPool *pool;
    tswPipe *pipe;

    serv->onMasterStart();
    pool = (tswProcessPool *)malloc(sizeof(tswProcessPool));
    if (pool == NULL) {
        tswWarn("%s", "malloc error");
        return TSW_ERR;
    }
    if (tswProcessPool_create(pool, serv->worker_num) < 0) {
        tswWarn("%s", "tswProcessPool_create error");
        return TSW_ERR;
    }

    for (int i = 0; i < serv->worker_num; i++) {
        tswPipeUnsock *object;

        pipe = &pool->pipes[i];

        if (tswPipeUnsock_create(pipe) < 0) {
            tswWarn("%s", "tswPipeUnsock_create error");
            return TSW_ERR;
        }
        pool->workers[i].pipe_master = pipe->getFd(pipe, TSW_PIPE_MASTER);
        pool->workers[i].pipe_worker = pipe->getFd(pipe, TSW_PIPE_WORKER);
        pool->workers[i].pipe_object = pipe;
    }

    for (int i = 0; i < serv->worker_num; i++) {
        if (tswServer_create_worker(serv, pool, i) < 0) {
            tswWarn("%s", "tswServer_create_worker error");
            return TSW_ERR;
        }
    }

    tswProcessPool_info(pool);
    serv->process_pool = pool;

    if (tswServer_start_proxy(serv) < 0) {
        tswWarn("%s", "tswServer_start_proxy error");
        return TSW_ERR;
    }

    return TSW_OK;
}

/*
 * reactor: Used to manage handle in tswEvent
*/
int tswServer_master_onAccept(tswReactor *reactor, tswEvent *tswev)
{
    int connfd;
    socklen_t len;
    struct sockaddr_in cliaddr;
    tswServer *serv = reactor->ptr;
    tswReactor *sub_reactor;

    len = sizeof(cliaddr);
    connfd = accept(tswev->fd, (struct sockaddr *)&cliaddr, &len);
    if (connfd < 0) {
        tswWarn("%s", "accept error");
        return TSW_ERR;
    }

    serv->status->accept_count++;

    sub_reactor = &(serv->reactor_threads[connfd % serv->reactor_num].reactor);

    serv->connection_list[connfd].connfd = connfd;
    serv->connection_list[connfd].session_id = serv->status->accept_count;
    serv->connection_list[connfd].from_reactor_id = sub_reactor->id;
    serv->connection_list[connfd].serv_sock = serv->serv_sock;

    serv->session_list[serv->status->accept_count].session_id = serv->status->accept_count;
    serv->session_list[serv->status->accept_count].connfd = connfd;
    serv->session_list[serv->status->accept_count].reactor_id = sub_reactor->id;
    serv->session_list[serv->status->accept_count].serv_sock = serv->serv_sock;

    serv->onConnect(serv->status->accept_count);

    if (sub_reactor->add(sub_reactor, connfd, TSW_EVENT_READ, tswServer_reactor_onReceive) < 0) {
        tswWarn("%s", "reactor add error");
        return TSW_ERR;
    }

    return TSW_OK;
}

int tswServer_reactor_onReceive(tswReactor *reactor, tswEvent *tswev)
{
    int n;
    int pipe_master;
    char buffer[MAX_BUF_SIZE];
    tswReactorEpoll *reactor_epoll_object = reactor->object;
    tswEventData event_data;
    int worker_id;

    n = recv(tswev->fd, event_data.data, TSW_BUFFER_SIZE, 0);
    if (n == 0) {
        reactor->del(reactor, tswev->fd);
        close(tswev->fd);
        free(tswev);
        reactor->event_num -= 1;
        return TSW_OK;
    }

    event_data.info.len = n;
    event_data.info.from_id = reactor->id;
    event_data.info.fd = TSwooleG.serv->connection_list[tswev->fd].session_id;
    worker_id = tswev->fd % TSwooleG.serv->process_pool->workers_num;

    pipe_master = TSwooleG.serv->process_pool->workers[worker_id].pipe_master;
    write(pipe_master, (void *)&event_data, sizeof(event_data.info) + event_data.info.len);

    if (reactor->add(reactor, pipe_master, TSW_EVENT_READ, tswReactor_onPipeReceive) < 0) {
        tswWarn("%s", "reactor add error");
        return TSW_ERR;
    }

    return TSW_OK;
}

void tswServer_master_onStart(void)
{
    tswDebug("%s", "master thread started successfully");
}

void tswServer_reactor_onStart(int reactor_id)
{
    tswDebug("reactor thread [%d] started successfully", reactor_id);
}

int tswReactor_onPipeReceive(tswReactor *reactor, tswEvent *tswev)
{
    int n;
    int session_id;
    int connfd;
    tswEventData event_data;
    tswSession *session;

    // tswev->fd represents the fd of the pipe
    n = read(tswev->fd, &event_data, sizeof(event_data));
    session_id = event_data.info.fd;
    session = &(TSwooleG.serv->session_list[session_id]);

    send(session->connfd, event_data.data, event_data.info.len, 0);
    if (reactor->del(reactor, tswev->fd) < 0) {
        tswWarn("%s", "reactor del error");
        return TSW_ERR;
    }

    return TSW_OK;
}

/*
 * worker process: 
 * 
 * @fd: session_id
 * @data: data from the send() method of php user space
*/
int tswServer_tcp_send(tswServer *serv, int fd, const void *data, size_t length)
{
    tswEventData event_data;

    event_data.info.len = length;
    event_data.info.from_id = TSwooleWG.id;
    event_data.info.fd = fd;
    memcpy(event_data.data, data, length);

    tswWorker_sendToReactor(&event_data);
    return TSW_OK;
}
