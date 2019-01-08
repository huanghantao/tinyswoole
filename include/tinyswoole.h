#ifndef TINYSWOOLE_H_
#define TINYSWOOLE_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <time.h>

#define TSW_OK 0
#define TSW_ERR -1

// Read and write events of interest to the file descriptor
enum tswEvent_type {
	TSW_EVENT_READ = 0,
	TSW_EVENT_WRITE = 1,
};

typedef struct _tswEvent tswEvent;
typedef struct _tswServer tswServer;
typedef struct _tswDataHead tswDataHead;
typedef struct _tswEventData tswEventData;
typedef struct _tswServerG tswServerG;
typedef struct _tswThreadParam tswThreadParam;
typedef struct _tswReactor tswReactor;
typedef struct _tswReactorEpoll tswReactorEpoll;
typedef struct _tswReactorThread tswReactorThread;
typedef struct _tswWorker tswWorker;
typedef struct _tswProcessPool tswProcessPool;
typedef struct _tswServerStatus tswServerStatus;
typedef struct _tswConnection tswConnection;
typedef struct _tswSession tswSession;
typedef struct _tswWorkerG tswWorkerG;

typedef struct _tswPipe tswPipe;
typedef struct _tswPipeUnsock tswPipeUnsock;

#define TSW_IPC_MAX_SIZE 8192
#define TSW_BUFFER_SIZE (TSW_IPC_MAX_SIZE - sizeof(tswDataHead))

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

struct _tswDataHead {
	uint16_t len;	// data len
	uint16_t from_id; // reactor id
	uint16_t fd; // session id
};

struct _tswEventData {
	tswDataHead info;
	char data[TSW_BUFFER_SIZE];
};

/*
 * Used to manage handles
 * 
 * add: Add a handle and register the event of interest and the event handler that was executed when the event occurred.
 * set: Set an event of interest to a handle
 * del: Delete a handle
*/
struct _tswReactor {
	void *object; // event object, for example, tswReactorEpoll
	void *ptr;  // reserve(to server)
	int event_num;
	int max_event_num;

	tswEvent tswev[MAXEVENTS + 1];

	// fd is handle instead of session_id
	int (*add)(tswReactor *reactor, int fd, int tsw_event_type, int (*tswReactor_handler)(tswReactor *reactor, tswEvent *tswev));
	int (*set)(tswReactor *reactor, int fd, int event_type);
	int (*del)(tswReactor *reactor, int fd);
	
	int (*wait)(tswReactor *reactor); // Waiting for events
	int (*free)(tswReactor *reactor); // free the reactor manager
	int (*setHandler)(tswEvent *tswev, int (*tswReactor_handler)(tswReactor *reactor, tswEvent *tswev)); // set event handler

	uint16_t id; // reactor id
};

int tswReactor_create(tswReactor *reactor, int max_event_num);
int tswReactorEpoll_create(tswReactor *reactor, int max_event_num);
int tswReactor_setHandler(tswEvent *tswev, int (*tswReactor_handler)(tswReactor *reactor, tswEvent *tswev));

struct _tswConnection {
	int connfd;
	uint32_t session_id;
	uint32_t from_reactor_id;
	int serv_sock;
};

struct _tswSession {
    uint32_t session_id;
    int connfd;
    uint32_t reactor_id;
	int serv_sock;
};

struct _tswServerStatus {
    uint32_t accept_count;
};

/*
 * in every worker process
*/
struct _tswWorkerG {
    uint32_t id; // worker_id
	int pipe_master;
	int pipe_worker;
};

struct _tswPipe {
	void *object;

	int (*read)(tswPipe *pipe, void *recv, int length);
    int (*getFd)(tswPipe *pipe, int isWriteFd);
	int (*write)(tswPipe *pipe, void *send, int length);
};

struct _tswPipeUnsock {
    int socks[2]; // socks[0]: worker, socks[1]: master
};

int tswPipeUnsock_create(tswPipe *pipe);

#endif /* TINYSWOOLE_H_ */
