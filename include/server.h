#ifndef TSW_SERVER_H_
#define TSW_SERVER_H_

#include "tinyswoole.h"

struct _tswServer {
    void (*onStart)(void);
    void (*onConnect)(int fd);
    void (*onReceive)(tswServer *serv, int fd, char *data);
    void (*onClose)(void);
};

int start(tswServer *serv, int sock);
int tswServer_tcp_send(tswServer *serv, int fd, const void *data, size_t length);

#endif /* TSW_SERVER_H_ */
