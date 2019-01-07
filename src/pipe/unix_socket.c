#include <stdlib.h>
#include "../../include/tinyswoole.h"

int tswPipeUnsock_create(tswPipeBase *pipe)
{
    tswPipeUnsock *object;

    object = (tswPipeUnsock *)malloc(sizeof(tswPipeUnsock));
    if (pipe->object == NULL) {
        tswWarn("malloc error");
		return TSW_ERR;
    }

    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, object->socks) < 0) {
        tswWarn("socketpair error");
        free(object);
		return TSW_ERR;
    }

    pipe->object = object;

    return TSW_OK;
}