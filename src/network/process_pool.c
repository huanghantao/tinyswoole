#include "../../include/process_pool.h"
#include "../../include/worker.h"
#include "../../include/server.h"

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
	int pipefd1[2];
    int pipefd2[2];
    tswWorker *worker;

	if (pipe(pipefd1) < 0) {
		tswWarn("%s", "pipe error");
		return TSW_ERR;
	}
    if (pipe(pipefd2) < 0) {
		tswWarn("%s", "pipe error");
		return TSW_ERR;
	}

	pid = fork();
	if (pid > 0) { // master process
        worker = pool->workers + worker_id;
        worker->pid = pid;
        pool->workers_num++;
        worker->worker_id = worker_id;

		close(pipefd1[0]);
        worker->write_pipefd = pipefd1[1];
        close(pipefd2[1]);
        worker->read_pipefd = pipefd2[0];
		return TSW_OK;
	}

    // worker process
	close(pipefd1[1]);
	close(pipefd2[0]);
    // pipefd1[0] is used to read the data sent by the reactor thread
    // pipefd2[1] is used to send the data to the reactor thread
    TSwooleWG.read_pipefd = pipefd1[0];
    TSwooleWG.write_pipefd = pipefd2[1];
    TSwooleWG.id = worker_id;
	tswWorker_loop(worker_id);
	
	return TSW_OK;
}

void tswProcessPool_info(const tswProcessPool *pool)
{
    tswWorker *worker = pool->workers;
    for (int i = 0; i < pool->workers_num; i++) {
        tswDebug("worker [%d] pid is %d", worker[i].worker_id, worker[i].pid);
    }
}