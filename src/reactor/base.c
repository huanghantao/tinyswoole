#include "../../include/tinyswoole.h"

int tswReactor_create(tswReactor *reactor, int max_event_num)
{
    int ret;

    ret = tswReactorEpoll_create(reactor, max_event_num);

    // reactor->write = tswReactor_write;
    // reactor->read = tswReactor_read;
    // reactor->close = tswReactor_close;
    
    return ret;
}