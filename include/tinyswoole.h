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

typedef struct _tswServer tswServer;
typedef struct _tswDataHead tswDataHead;
typedef struct _tswEventData tswEventData;
typedef struct _tswReactor tswReactor;
typedef struct _tswWorker tswWorker;

#define TSW_IPC_MAX_SIZE            8192
#define TSW_BUFFER_SIZE             (TSW_IPC_MAX_SIZE - sizeof(tswDataHead))

struct _tswDataHead {
    uint16_t len; // data len
    int16_t from_id; // reactor id
};

struct _tswEventData {
    tswDataHead info;
    char data[TSW_BUFFER_SIZE];
} tswEventData;

struct _tswReactor {
	pthread_t tid;
} tswReactor;

struct _tswWorker {
	int pid;
} tswWorker;

#endif /* TINYSWOOLE_H_ */
