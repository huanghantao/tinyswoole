#ifndef TSW_SERVER_H_
#define TSW_SERVER_H_

#include "tinyswoole.h"

#define LISTENQ 10
#define MAX_BUF_SIZE 1024

tswServerG TSwooleG;
int listen_epollfd;
int conn_epollfd;

struct _tswServer {
    void (*onStart)(void);
    void (*onConnect)(int fd);
    void (*onReceive)(tswServer *serv, int fd, char *data);
    void (*onClose)(void);
};

tswServer *tswServer_new(void);
int start(tswServer *serv, int listenfd);
int tswServer_master_loop(tswServer *serv, int listenfd);
int tswServer_master_onAccept(const tswEvent *event);
int tswServer_tcp_send(tswServer *serv, int fd, const void *data, size_t length);
int tswReactorThread_start(tswServer *serv, int listenfd);

#endif /* TSW_SERVER_H_ */
