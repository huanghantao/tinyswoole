#ifndef PROCESS_H_
#define PROCESS_H_

#include "tinyswoole.h"

struct _tswReactor {
	pthread_t tid;
};

struct _tswWorker {
	int pid;
};

/*
 * Master线程对连接进行accept
*/
int masterHandler(int epollfd);


#endif /* PROCESS_H_ */
