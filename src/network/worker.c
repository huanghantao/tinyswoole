#include "../../include/worker.h"
#include "../../include/epoll.h"
#include "../../include/server.h"

static int tswWorker_onPipeReceive(tswReactor *reactor, tswEvent *tswev)
{
    int n;
    char buffer[MAX_BUF_SIZE];

    n = read(tswev->fd, buffer, MAX_BUF_SIZE);
    if (n > 0) {
        tswDebug("worker process receive: %s", buffer);
    }
}

int tswWorker_loop(int worker_id, int sockfd)
{
    tswReactor *main_reactor;

    main_reactor = malloc(sizeof(tswReactor));
	if (main_reactor == NULL) {
		tswWarn("%s", "malloc error");
		return TSW_ERR;
	}

	if (tswReactor_create(main_reactor, MAXEVENTS) < 0) {
		tswWarn("%s", "tswReactor_create error");
		return TSW_ERR;
	}

    if (main_reactor->add(main_reactor, sockfd, TSW_EVENT_READ, tswWorker_onPipeReceive) < 0) {
		tswWarn("%s", "reactor add error");
		return TSW_ERR;
	}

    for (;;) {
		int nfds;

		nfds = main_reactor->wait(main_reactor);
		for (int i = 0; i < nfds; i++) {
			int connfd;
		    tswReactorEpoll *reactor_epoll_object = main_reactor->object;

			tswEvent *tswev = (tswEvent *)reactor_epoll_object->events[i].data.ptr;
			if (tswev->event_handler(main_reactor, tswev) < 0) {
				tswWarn("%s", "event_handler error");
				continue;
			}
		}
	}
}
