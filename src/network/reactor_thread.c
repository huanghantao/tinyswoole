#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../include/server.h"
#include "../../include/epoll.h"


static int tswReactorThread_loop(tswThreadParam *param);

static int tswReactorThread_loop(tswThreadParam *param)
{
    int nfds;
    int listenfd = param->listenfd;
    int reactor_id = param->pti;
    tswServer *serv = param->object;
	char buffer[MAX_BUF_SIZE];
    struct epoll_event *events;

	events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAXEVENTS);
	if (events == NULL) {
		printf("malloc error\n");
		return TSW_ERR;
	}

    for (;;) {
	    nfds = epoll_wait(conn_epollfd, events, MAXEVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            if (events[i].events & EPOLLIN && events[i].data.fd != listenfd) {
                int n;

                n = read(events[i].data.fd, buffer, MAX_BUF_SIZE);
                if (n == 0) {
                    epoll_del(conn_epollfd, events[i].data.fd);
                    close(events[i].data.fd);
                    continue;
                }
                buffer[n] = 0;
                if (serv->onReceive != NULL) {
                    serv->onReceive(serv, events[i].data.fd, buffer);
                }
                continue;
			}
        }
    }
}

int tswReactorThread_start(tswServer *serv, int listenfd)
{
    pthread_t pidt;
    tswThreadParam *param;

    param = (tswThreadParam *)malloc(sizeof(*param));
    if (param == NULL) {
        return TSW_ERR;
    }
    param->listenfd = listenfd;
    param->object = serv;

    for (int i = 0; i < 4; i++) {
        param->pti = i;
        pthread_create(&pidt, NULL, (void * (*)(void *))tswReactorThread_loop, (void *)param);
    }

    return TSW_OK;
}