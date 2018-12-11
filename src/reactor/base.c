#include "../../include/tinyswoole.h"

int tswReactor_create(tswReactor *reactor, int max_event_num)
{
    if (tswReactorEpoll_create(reactor, max_event_num) < 0) {
        tswWarn("tswReactorEpoll_create error.");
		return TSW_ERR;
    }
    reactor->setHandler = tswReactor_setHandler;
    return TSW_OK;
}

int tswReactor_setHandler(tswEvent *tswev, int (*tswReactor_handler)(tswReactor *reactor, tswEvent *tswev))
{
    tswev->event_handler = tswReactor_handler;
    return TSW_OK;
}