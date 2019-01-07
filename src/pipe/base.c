#include <stdlib.h>
#include "../../include/tinyswoole.h"

int tswPipeBase_create(tswPipeBase *pipe)
{
    pipe = (tswPipeBase *)malloc(sizeof(tswPipeBase));
    if (pipe == NULL) {
        tswWarn("tswPipeBase_create error.");
		return TSW_ERR;
    }

    return TSW_OK;
}