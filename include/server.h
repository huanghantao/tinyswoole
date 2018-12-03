#ifndef TSW_SERVER_H_
#define TSW_SERVER_H_

#include "tinyswoole.h"

#define LISTENQ 10
#define MAX_BUF_SIZE 1024
#define MAXEVENTS 64

struct _tswServer {
    void (*onStart)(void);
    void (*onConnect)(int fd);
    void (*onReceive)(tswServer *serv, int fd, char *data);
    void (*onClose)(void);
};

tswServer * tswServer_new(void);
int start(tswServer *serv, int listenfd);
int tswServer_tcp_send(tswServer *serv, int fd, const void *data, size_t length);

#endif /* TSW_SERVER_H_ */
