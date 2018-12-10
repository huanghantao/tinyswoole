#ifndef EPOLL_H_
#define EPOLL_H_

#include <sys/epoll.h>
#include "tinyswoole.h"


int epoll_add(tswReactor *reactor, int fd, int event_type, int (*tswReactor_handler)(int fd));
int epoll_del(int epollfd, int fd);
int epoll_set_output(int epollfd, int fd);
int epoll_event_set(int epollfd, int fd, int event_type);

#endif /* EPOLL_H_ */
