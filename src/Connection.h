//
// Created by comoon on 5/31/17.
//

#ifndef ZEUS_CONNECTION_H
#define ZEUS_CONNECTION_H

#include <memory>
#include <netinet/in.h>
#include <unistd.h>
#include "ObjectPool.h"

struct Connection : PoolObject<Connection>{
    int _fd;
    struct sockaddr_in _sockAddr;
    socklen_t _addr_len{sizeof(_sockAddr)};

    Connection(){}
    ~Connection(){ ::close(_fd); }
};

typedef std::shared_ptr<Connection> connectionPtr;



#endif //ZEUS_CONNECTION_H
