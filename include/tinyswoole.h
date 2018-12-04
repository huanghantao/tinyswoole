#ifndef TINYSWOOLE_H_
#define TINYSWOOLE_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "log.h"


#define TSW_OK   0
#define TSW_ERR  -1
#define TSW_ERROR_MSG_SIZE          512

char tsw_error[TSW_ERROR_MSG_SIZE];

#define tswWarn(str,...)  snprintf(tsw_error,TSW_ERROR_MSG_SIZE,"%s: " str,__func__,##__VA_ARGS__);\
tswLog_put(TSW_LOG_WARNING,tsw_error);

enum tswFd_type {
    TSW_FD_LISTEN          = 0, // server socket
    TSW_FD_WRITE          = 1, // fd can write
    TSW_FD_READ          = 2, // fd can read
};

// Read and write events of interest to the file descriptor
enum tswEvent_type {
    TSW_EVENT_READ = 0,
    TSW_EVENT_WRITE = 1,
};

typedef struct _tswEvent tswEvent;
typedef struct _tswServer tswServer;
typedef struct _tswDataHead tswDataHead;
typedef struct _tswEventData tswEventData;
typedef struct _tswWorker tswWorker;
typedef struct _tswServerG tswServerG;
typedef struct _tswThreadParam tswThreadParam;
typedef struct _tswReactor tswReactor;
typedef struct _tswReactorEpoll tswReactorEpoll;

#define TSW_IPC_MAX_SIZE            8192
#define TSW_BUFFER_SIZE             (TSW_IPC_MAX_SIZE - sizeof(tswDataHead))

typedef struct _tswEvent {
    int fd;
} tswEvent;

struct _tswDataHead {
    uint16_t len; // data len
    int16_t from_id; // reactor id
};

struct _tswEventData {
    tswDataHead info;
    char data[TSW_BUFFER_SIZE];
};

struct _tswWorker {
	int pid;
};

struct _tswServerG {
    tswServer *serv;
};

struct _tswThreadParam {
    void *object;
    int listenfd;
	int pti;
};

struct _tswReactorEpoll {
    int epfd;
    struct epoll_event *events; // for epoll_wait()
};

struct _tswReactor {
    void *object; // event object, for example, tswReactorEpoll
    uint32_t event_num;
    uint32_t max_event_num;

    int (*add)(tswReactor *reactor, int fd, int event_type);
    int (*set)(tswReactor *reactor, int fd, int event_type);
    int (*del)(tswReactor *reactor, int fd);
    int (*wait)(tswReactor *reactor);
    int (*free)(tswReactor *reactor);

    int (*write)(tswReactor *reactor, int fd, void *buf, int n); // // An interface that sends data to a socket using reactor
    int (*close)(tswReactor *reactor, int fd);
};

int tswReactor_create(tswReactor *reactor, int max_event_num);
int tswReactorEpoll_create(tswReactor *reactor, int max_event_num);

#endif /* TINYSWOOLE_H_ */
