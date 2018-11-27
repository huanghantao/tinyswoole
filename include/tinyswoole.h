#ifndef TINYSWOOLE_H_
#define TINYSWOOLE_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "log.h"


#define TSW_OK   0
#define TSW_ERR  -1
#define TSW_ERROR_MSG_SIZE          512

char tsw_error[TSW_ERROR_MSG_SIZE];


#define tswWarn(str,...)  snprintf(tsw_error,TSW_ERROR_MSG_SIZE,"%s: " str,__func__,##__VA_ARGS__);\
tswLog_put(TSW_LOG_WARNING,tsw_error);

typedef struct _tswServer tswServer;

#endif /* TINYSWOOLE_H_ */
