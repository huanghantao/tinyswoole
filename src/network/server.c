#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "tswoole_config.h"
#include "../../include/server.h"
#include "../../include/epoll.h"

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

	return serv;
}

int start(tswServer *serv, int listenfd)
{
	listen(listenfd, LISTENQ);
	if (serv->onStart != NULL) {
		serv->onStart();
	}

	tswReactor *reactor = malloc(sizeof(tswReactor));
	if (reactor == NULL) {
		tswWarn("malloc error.");
		return TSW_ERR;
	}

	if (tswReactor_create(reactor, MAXEVENTS) < 0) {
		tswWarn("tswReactor_create error.");
		return TSW_ERR;
	}

	if (reactor->add(reactor, listenfd, TSW_EVENT_READ, tswServer_master_onAccept) < 0) {
		tswWarn("reactor add error.");
		return TSW_ERR;
	}

	for (;;) {
		int nfds;

		nfds = reactor->wait(reactor);

		for (int i = 0; i < nfds; i++) {
			int connfd;
		    tswReactorEpoll *reactor_epoll_object = reactor->object;

			tswEvent *tswev = (tswEvent *)reactor_epoll_object->events[i].data.ptr;
			if (tswev->event_handler(reactor, tswev) < 0) {
				tswWarn("event_handler error");
				continue;
			}
		}
	}

	close(listenfd);
}

int tswServer_master_onAccept(tswReactor *reactor, tswEvent *tswev)
{
	int connfd;
	socklen_t len;
	struct sockaddr_in cliaddr;

	len = sizeof(cliaddr);
	connfd = accept(tswev->fd, (struct sockaddr *)&cliaddr, &len);
	TSwooleG.serv->onConnect(connfd);

	if (reactor->add(reactor, connfd, TSW_EVENT_READ, tswServer_master_onReceive) < 0) {
		tswWarn("reactor add error.");
		return TSW_ERR;
	}

	return TSW_OK;
}

int tswServer_master_onReceive(tswReactor *reactor, tswEvent *tswev)
{
	int n;
	char buffer[MAX_BUF_SIZE];
	tswReactorEpoll *reactor_epoll_object = reactor->object;

	n = read(tswev->fd, buffer, MAX_BUF_SIZE);
	if (n == 0) {
		reactor->del(reactor, tswev);
		close(tswev->fd);
		free(tswev);
		return TSW_OK;
	}
	buffer[n] = 0;
	TSwooleG.serv->onReceive(TSwooleG.serv, tswev->fd, buffer);

	return TSW_OK;
}

int tswServer_tcp_send(tswServer *serv, int fd, const void *data, size_t length)
{
	return send(fd, data, length, 0);
}
