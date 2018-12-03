#ifndef EPOLL_H_
#define EPOLL_H_

#include <sys/epoll.h>
#include "tinyswoole.h"


int epoll_add(int epollfd, int fd, int event_type, int flag);
int epoll_del(int epollfd, int fd);
int epoll_set_output(int epollfd, int fd);
int epoll_event_set(int epollfd, int fd, int event_type);

#endif /* EPOLL_H_ */
