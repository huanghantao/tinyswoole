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
#include "../include/server.h"
#include "../../include/epoll.h"

tswServer * tswServer_new(void)
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
	int epollfd;
	socklen_t len;
	struct sockaddr_in cliaddr;
	char buffer[MAX_BUF_SIZE];
	struct epoll_event *events;

	listen(listenfd, LISTENQ);
	if (serv->onStart != NULL) {
		serv->onStart();
	}
	len = sizeof(cliaddr);

	epollfd = epoll_create(512);
	if (epollfd < 0) {
		perror("epoll_create error: ");
		return TSW_ERR;
	}

	if (epoll_add(epollfd, listenfd, EPOLLIN | EPOLLET, 0) < 0) {
		printf("epoll_add error\n");
		return TSW_ERR;
	}

	events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAXEVENTS);
	if (events == NULL) {
		printf("malloc error\n");
		return TSW_ERR;
	}

	for (;;) {
		int nfds;

		nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == listenfd) {
				int connfd;

				connfd = accept(events[i].data.fd, (struct sockaddr *)&cliaddr, &len);
				if (connfd > 0) {
					if (epoll_add(epollfd, connfd, EPOLLIN | EPOLLET, 1) < 0) {
						printf("epoll_add error\n");
						return TSW_ERR;
					}
					if (serv->onConnect != NULL) {
						serv->onConnect(connfd);
					}
					continue;
				}
			}
			if (events[i].events & EPOLLIN) {
					int n;

					n = read(events[i].data.fd, buffer, MAX_BUF_SIZE);
					if (n == 0) {
						epoll_del(epollfd, events[i].data.fd);
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

	close(listenfd);
}

int tswServer_tcp_send(tswServer *serv, int fd, const void *data, size_t length)
{
	return send(fd, data, length, 0);
}
