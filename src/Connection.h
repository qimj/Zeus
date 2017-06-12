//
// Created by comoon on 5/31/17.
//

#ifndef ZEUS_CONNECTION_H
#define ZEUS_CONNECTION_H

#include <memory>
#include "ObjectPool.h"

struct Connection : PoolObject<Connection>{
    int _fd;
    struct sockaddr_in _sockAddr;
    socklen_t _addr_len{sizeof(_sockAddr)};
    std::string _ip;
    int64_t _lastActive {60};

    Connection(){}
    ~Connection(){ ::close(_fd); }
    void setIp() {
        std::shared_ptr<char> buf(new char[16], [](char *p){ delete []p; });
        inet_ntop(AF_INET, &_sockAddr, buf.get(), 16);
        _ip = std::string(buf.get());
    }
};

typedef std::shared_ptr<Connection> connectionPtr;

inline bool operator < (const std::weak_ptr<Connection> c1, const std::weak_ptr<Connection> c2) {
    return true;
}


#endif //ZEUS_CONNECTION_H
