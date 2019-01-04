#ifndef WORKER_H_
#define WORKER_H_

#include <stdlib.h>
#include <unistd.h>
#include "tinyswoole.h"

int tswWorker_sendToReactor(tswEventData *event_data);
int tswWorker_loop();

#endif /* WORKER_H_ */
