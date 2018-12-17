#include <stdlib.h>
#include "../../include/server.h"

int tswReactorThread_create(tswServer *serv)
{
    serv->reactor_threads = (tswReactorThread *)malloc(sizeof(tswReactorThread) * serv->reactor_num);
    if (serv->reactor_threads == NULL) {
        tswWarn("%s", "malloc error");
        return TSW_ERR;
    }
}