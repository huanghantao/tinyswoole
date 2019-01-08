#ifndef SOCKET_H_
#define SOCKET_H_

int tswSocket_create(int type);

enum tswSocket_type {
    TSW_SOCK_TCP          =  1,
    TSW_SOCK_UDP          =  2,
};

int tswSocket_create(int type);
int tswSocket_bind(int sock, int type, char *host, int port);

#endif /* SOCKET_H_ */