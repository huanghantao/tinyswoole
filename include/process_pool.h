#ifndef PROCESS_POOL_H_
#define PROCESS_POOL_H_

#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include "tinyswoole.h"

struct _tswWorker {
	pid_t pid;
	int worker_id;
};

struct _tswProcessPool {
	tswWorker *workers;
};

int tswProcessPool_create(tswProcessPool *pool, int worker_num);
int tswServer_create_worker(tswServer *serv, int worker_id);

#endif /* PROCESS_POOL_H_ */
