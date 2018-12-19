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

#define TSW_OK 0
#define TSW_ERR -1
#define TSW_ERROR_MSG_SIZE 512

char tsw_error[TSW_ERROR_MSG_SIZE];

#define tswWarn(str, ...)                                                         \
	snprintf(tsw_error, TSW_ERROR_MSG_SIZE, "%s: "str" in %s on line %d.", __func__, ##__VA_ARGS__, __FILE__, __LINE__); \
	tswLog_put(TSW_LOG_WARNING, tsw_error);

enum tswFd_type {
	TSW_FD_LISTEN = 0, // server socket
	TSW_FD_WRITE = 1,  // fd can write
	TSW_FD_READ = 2,   // fd can read
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
typedef struct _tswReactorThread tswReactorThread;

#define TSW_IPC_MAX_SIZE 8192
#define TSW_BUFFER_SIZE (TSW_IPC_MAX_SIZE - sizeof(tswDataHead))

struct _tswDataHead {
	uint16_t len;	// data len
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
	int pti; // Used to mark the threads, counting from 0
};

struct _tswReactorEpoll {
	int epfd;
	struct epoll_event *events; // for epoll_wait()
};

#define MAXEVENTS 64

typedef struct _tswEvent {
	int fd;
	int event;
	int (*event_handler)(tswReactor *reactor, tswEvent *tswev); // specific event handler
} tswEvent;

/*
 * Used to manage handles
 * 
 * add: Add a handle and register the event of interest and the event handler that was executed when the event occurred.
 * set: Set an event of interest to a handle
 * del: Delete a handle
*/
struct _tswReactor {
	void *object; // event object, for example, tswReactorEpoll
	int event_num;
	int max_event_num;

	tswEvent tswev[MAXEVENTS + 1];

	int (*add)(tswReactor *reactor, int fd, int tsw_event_type, int (*tswReactor_handler)(tswReactor *reactor, tswEvent *tswev));
	int (*set)(tswReactor *reactor, int fd, int event_type);
	int (*del)(tswReactor *reactor, int fd);
	
	int (*wait)(tswReactor *reactor); // Waiting for events
	int (*free)(tswReactor *reactor); // free the reactor manager
	int (*setHandler)(tswEvent *tswev, int (*tswReactor_handler)(tswReactor *reactor, tswEvent *tswev)); // set event handler
};

int tswReactor_create(tswReactor *reactor, int max_event_num);
int tswReactorEpoll_create(tswReactor *reactor, int max_event_num);
int tswReactor_setHandler(tswEvent *tswev, int (*tswReactor_handler)(tswReactor *reactor, tswEvent *tswev));

#endif /* TINYSWOOLE_H_ */
