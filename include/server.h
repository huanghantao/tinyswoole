#ifndef TSW_SERVER_H_
#define TSW_SERVER_H_

#include "tinyswoole.h"

struct _tswServer {
    void (*onStart)(void);
    void (*onConnect)(int fd);
    void (*onReceive)(void);
    void (*onClose)(void);
};

int start(tswServer *serv, int sock);

#endif /* TSW_SERVER_H_ */
