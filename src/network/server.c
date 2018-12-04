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

	listen_epollfd = epoll_create(512);
	if (listen_epollfd < 0) {
		perror("epoll_create error: ");
		return TSW_ERR;
	}

	if (epoll_add(listen_epollfd, listenfd, EPOLLIN | EPOLLET, 0) < 0) {
		printf("epoll_add error\n");
		return TSW_ERR;
	}

	conn_epollfd = epoll_create(512);
	if (conn_epollfd < 0) {
		perror("epoll_create error: ");
		return TSW_ERR;
	}

	tswReactorThread_start(serv, listenfd);
	tswServer_master_loop(serv, listenfd);

	close(listenfd);
}

int tswServer_master_loop(tswServer *serv, int listenfd)
{
	int nfds;
	struct epoll_event *events;

	events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAXEVENTS);
	if (events == NULL) {
		printf("malloc error\n");
		return TSW_ERR;
	}

	for (;;) {
		nfds = epoll_wait(listen_epollfd, events, MAXEVENTS, -1);
		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == listenfd) {
				int connfd;
				tswEvent event;

				event.fd = events[i].data.fd;
				connfd = tswServer_master_onAccept(&event);
				if (connfd > 0) {
					if (epoll_add(conn_epollfd, connfd, EPOLLIN | EPOLLET, 1) < 0) {
						printf("epoll_add error\n");
						return TSW_ERR;
					}
				}

				if (serv->onConnect != NULL) {
					serv->onConnect(connfd);
				}
				continue;
			}
		}
	}
}

int tswServer_master_onAccept(const tswEvent *event)
{
	int connfd;
	socklen_t len;
	struct sockaddr_in cliaddr;

	len = sizeof(cliaddr);
	connfd = accept(event->fd, (struct sockaddr *)&cliaddr, &len);
	
	return connfd;
}

int tswServer_tcp_send(tswServer *serv, int fd, const void *data, size_t length)
{
	return send(fd, data, length, 0);
}
