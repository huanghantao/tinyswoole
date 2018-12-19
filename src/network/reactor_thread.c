#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../../include/server.h"
#include "../../include/epoll.h"

static int tswReactorThread_loop(tswThreadParam *param)
{
    int pti = param->pti;
    tswServer *serv = param->object;
    tswReactor *reactor = &(serv->reactor_threads[pti].reactor);

    for (;;) {
        int nfds;

		nfds = reactor->wait(reactor);
        for (int i = 0; i < nfds; i++) {
			int connfd;
		    tswReactorEpoll *reactor_epoll_object = reactor->object;

			tswEvent *tswev = (tswEvent *)reactor_epoll_object->events[i].data.ptr;
            if (tswev->event_handler(reactor, tswev) < 0) {
				tswWarn("%s", "event_handler error");
				continue;
			}
		}
    }
}

int tswReactorThread_create(tswServer *serv)
{
    tswReactorThread *thread;

    serv->reactor_threads = (tswReactorThread *)malloc(sizeof(tswReactorThread) * serv->reactor_num);
    if (serv->reactor_threads == NULL) {
        tswWarn("%s", "malloc error");
        return TSW_ERR;
    }

    for (int i = 0; i < serv->reactor_num; i++) {
        thread = &(serv->reactor_threads[i]);
        if (tswReactor_create(&(thread->reactor), MAXEVENTS) < 0) {
            tswWarn("%s", "tswReactor_create error");
            return TSW_ERR;
        }
    }

    return TSW_OK;
}

int tswReactorThread_start(tswServer *serv)
{
    pthread_t pidt;
    tswReactorThread *thread;
    tswThreadParam *param;

    for (int i = 0; i < serv->reactor_num; i++) {
        param = (tswThreadParam *)malloc(sizeof(tswThreadParam));
        if (param == NULL) {
            tswWarn("%s", "malloc error");
            return TSW_ERR;
        }

        thread = &(serv->reactor_threads[i]);
        param->pti = i;
        param->object = serv;

        if (pthread_create(&pidt, NULL, (void * (*)(void *))tswReactorThread_loop, (void *)param) < 0) {
            tswWarn("%s", "pthread_create error");
        }
        thread->thread_id = pidt;
    }

    return TSW_OK;
}