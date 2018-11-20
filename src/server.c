#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#define LISTENQ 10
#define MAX_BUF_SIZE 1024


int server(char *ip, int port)
{
    int listenfd;
    int connfd;
    int n;
    socklen_t len;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    char buffer[MAX_BUF_SIZE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_aton(ip, &(servaddr.sin_addr));
    servaddr.sin_port = htons(port);
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    for (;;) {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
        for (;;) {
            n = read(connfd, buffer, MAX_BUF_SIZE);
            if (n <= 0) {
                close(connfd);
                break;
            }
            write(connfd, buffer, n);  
        }
    }

    close(listenfd);
}
