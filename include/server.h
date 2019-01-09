#ifndef TSW_SERVER_H_
#define TSW_SERVER_H_

#include "tinyswoole.h"

#define LISTENQ 10
#define MAX_BUF_SIZE 1024

tswServerG TSwooleG;
tswWorkerG TSwooleWG;

struct _tswServer {
    int serv_sock;

    void (*onStart)(tswServer *serv);
    void (*onConnect)(int fd);
    void (*onReceive)(tswServer *serv, tswEventData *event_data);
    void (*onClose)(void);

    void (*onMasterStart)(void);

    int reactor_num;
    tswReactorThread *reactor_threads;
    void (*onReactorStart)(int reactor_id);

    int worker_num;
    tswProcessPool *process_pool;

    tswConnection *connection_list;
    tswSession *session_list;
    tswServerStatus *status;
    int reactor_pipe_num;
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
void tswServer_master_onStart(void);
void tswServer_reactor_onStart(int reactor_id);

int tswReactorThread_create(tswServer *serv);
int tswReactorThread_start(tswServer *serv);
int tswReactorThread_sendToWorker(tswServer *serv, tswEventData *event_data, int worker_id);
int tswReactorThread_onPipeReceive(tswReactor *reactor, tswEvent *tswev);

int tswServer_tcp_send(tswServer *serv, int fd, const void *data, size_t length);

enum tswWorkerPipeType {
    TSW_PIPE_WORKER     = 0,
    TSW_PIPE_MASTER     = 1,
};

#endif /* TSW_SERVER_H_ */
