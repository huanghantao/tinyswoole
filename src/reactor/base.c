#include "../../include/tinyswoole.h"

int tswReactor_create(tswReactor *reactor, int max_event_num)
{
    int ret;

    ret = tswReactorEpoll_create(reactor, max_event_num);

    reactor->setHandler = tswReactor_setHandler;
    
    return TSW_OK;
}

int tswReactor_setHandler(tswEvent *tswev, int (*tswReactor_handler)(int fd))
{
    tswev->event_handler = tswReactor_handler;
    return TSW_OK;
}