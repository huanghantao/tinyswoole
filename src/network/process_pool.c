#include "../../include/process_pool.h"

int tswProcessPool_create(tswProcessPool *pool, int worker_num)
{
    pool->workers = (tswWorker *)malloc(sizeof(tswWorker) * worker_num);
    if (pool->workers == NULL) {
        tswWarn("%s", "malloc error");
		return TSW_ERR;
    }

    return TSW_OK;
}

int tswServer_create_worker(tswServer *serv, int worker_id)
{
	pid_t pid;
	int sockfd[2];

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd) < 0) {
		tswWarn("%s", "socketpair error");
		return TSW_ERR;
	}

	pid = fork();
	if (pid > 0) {
		close(sockfd[1]);
		return pid;
	}

	close(sockfd[0]);
	for (;;) {
		tswDebug("worker process [%d] is running...", worker_id);
		sleep(1);
	}

	return TSW_OK;
}