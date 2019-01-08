#include "process_pool.h"
#include "worker.h"
#include "log.h"
#include "tinyswoole.h"
#include "server.h"


int tswProcessPool_create(tswProcessPool *pool, int worker_num)
{
    pool->workers = (tswWorker *)malloc(sizeof(tswWorker) * worker_num);
    if (pool->workers == NULL) {
        tswWarn("%s", "malloc error");
        return TSW_ERR;
    }
    pool->pipes = malloc(sizeof(tswPipe) * worker_num);
    if (pool->pipes == NULL) {
        tswWarn("%s", "malloc error");
		return TSW_ERR;
    }
    pool->worker_num = worker_num;

    return TSW_OK;
}

int tswServer_create_worker(tswServer *serv, tswProcessPool *pool, int worker_id)
{
    pid_t pid;
    tswWorker *worker;

    worker = pool->workers + worker_id;
    pid = fork();
    if (pid > 0) { // master process
        worker->pid = pid;
        worker->worker_id = worker_id;
        return TSW_OK;
    }

    // worker process
    TSwooleWG.pipe_master = worker->pipe_master;
    TSwooleWG.pipe_worker = worker->pipe_worker;
    TSwooleWG.id = worker_id;
    if (tswWorker_loop() < 0) {
        tswWarn("%s", "tswWorker_loop error");
        return TSW_ERR;
    }
	
    return TSW_OK;
}

void tswProcessPool_info(const tswProcessPool *pool)
{
    tswWorker *worker = pool->workers;
    for (int i = 0; i < pool->worker_num; i++) {
        tswDebug("worker [%d] pid is %d", worker[i].worker_id, worker[i].pid);
    }
}