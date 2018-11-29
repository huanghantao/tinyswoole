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


#define LISTENQ 10
#define MAX_BUF_SIZE 1024


int start(tswServer *serv, int sock)
{
	int connfd;
	int n;
	socklen_t len;
	struct sockaddr_in cliaddr;
	char buffer[MAX_BUF_SIZE];

	listen(sock, LISTENQ);
	len = sizeof(cliaddr);

    for (;;) {
    	connfd = accept(sock, (struct sockaddr *)&cliaddr, &len);
		if (connfd > 0) {
			serv->onConnect();
			for (;;) {
				n = read(connfd, buffer, MAX_BUF_SIZE);
				if (n <= 0) {
					close(connfd);
					break;
				}
				write(connfd, buffer, n);  
			}
		}
	}

	close(sock);
}
