#include "../../include/process_pool.h"

int tswProcessPool_create(tswProcessPool *pool, int worker_num)
{
    pool->workers = (tswWorker *)malloc(sizeof(tswWorker) * worker_num);
    if (pool->workers == NULL) {
        tswWarn("%s", "malloc error");
		return TSW_ERR;
    }
    pool->workers_num = 0;

    return TSW_OK;
}

int tswServer_create_worker(tswServer *serv, tswProcessPool *pool, int worker_id)
{
	pid_t pid;
	int sockfd[2];
    tswWorker *worker;

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd) < 0) {
		tswWarn("%s", "socketpair error");
		return TSW_ERR;
	}

	pid = fork();
	if (pid > 0) { // master process
        worker = pool->workers + worker_id;
		close(sockfd[1]);
        worker->pid = pid;
        worker->worker_id = worker_id;
        worker->sockfd = sockfd[0];
        pool->workers_num++;
		return TSW_OK;
	}

    // worker process
	close(sockfd[0]);
	for (;;) {
		// tswDebug("worker process [%d] is running...", worker_id);
		sleep(1);
	}
	return TSW_OK;
}

void tswProcessPool_info(const tswProcessPool *pool)
{
    tswWorker *worker = pool->workers;
    for (int i = 0; i < pool->workers_num; i++) {
        tswDebug("worker [%d] pid is %d", worker[i].worker_id, worker[i].pid);
    }
}