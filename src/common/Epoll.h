//
// Created by paoli on 16/8/20.
//

#ifndef ZEUS_EPOLL_H
#define ZEUS_EPOLL_H

#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_EVENT 10000

class Epoll {


private:
    struct epoll_event _ev, _events[MAX_EVENT];
};

#endif //ZEUS_EPOLL_H
