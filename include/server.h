#ifndef TSW_SERVER_H_
#define TSW_SERVER_H_

#include "tinyswoole.h"

#define LISTENQ 10
#define MAX_BUF_SIZE 1024

tswServerG TSwooleG;
int listen_epollfd;
int conn_epollfd;

struct _tswServer {
    int serv_sock;

    void (*onStart)(void);
    void (*onConnect)(int fd);
    void (*onReceive)(tswServer *serv, int fd, char *data);
    void (*onClose)(void);

    int reactor_num;
    tswReactorThread *reactor_threads;
};

/*
 * Used for the reactor thread instead of the master thread
*/
struct _tswReactorThread {
    pthread_t thread_id;
    tswReactor reactor;
};

tswServer *tswServer_new(void);
int tswServer_start(tswServer *serv);
int tswServer_master_loop(tswServer *serv, int listenfd);
int tswServer_master_onAccept(tswReactor *reactor, tswEvent *tswev);
int tswServer_reactor_onReceive(tswReactor *reactor, tswEvent *tswev);
int tswServer_tcp_send(tswServer *serv, int fd, const void *data, size_t length);

int tswReactorThread_create(tswServer *serv);
int tswReactorThread_start(tswServer *serv);

#endif /* TSW_SERVER_H_ */
