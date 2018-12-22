#ifndef WORKER_H_
#define WORKER_H_

#include <stdlib.h>
#include <unistd.h>
#include "tinyswoole.h"

int tswWorker_loop(int worker_id, int sockfd);

#endif /* WORKER_H_ */
