#ifndef TSW_SERVER_H_
#define TSW_SERVER_H_

#include "tinyswoole.h"

struct _tswServer {
    void (*onStart)(tswServer *serv);
    void (*onConnect)(tswServer *serv);
    int (*onReceive)(tswServer *);
    void (*onClose)(tswServer *serv);
};

int start(int sock);

#endif /* TSW_SERVER_H_ */
