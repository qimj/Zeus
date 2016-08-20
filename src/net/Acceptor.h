//
// Created by paoli on 16/8/18.
//

#ifndef ZEUS_ACCEPTOR_H
#define ZEUS_ACCEPTOR_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <folly/futures/Future.h>
#include "Connect.h"
#include "../common/FileDesc.h"

class Acceptor {

public:
    Acceptor(const Acceptor &) = delete;
    Acceptor(Acceptor &&) = delete;
    Acceptor &operator = (const Acceptor &) = delete;
    Acceptor &operator = (Acceptor && a) = delete;
    ~Acceptor() { _fd.shutdown(SHUT_RDWR); }

    Acceptor(unsigned port, const char * addr = nullptr) {

        _fd = FileDesc::socket(AF_INET, SOCK_STREAM, 0);
        bzero(&_sockAddr, sizeof(_sockAddr));
        _sockAddr.sin_family = AF_INET;
        _sockAddr.sin_port = htons(port);

        if(addr) {
            _sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        }
        else{
            inet_pton(AF_INET, addr, &_sockAddr.sin_addr);
        }

        _fd.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
        _fd.bind((struct sockaddr*) &_sockAddr, sizeof _sockAddr);
    }

    Future<Connect> doAccept(){

    }

private:
    struct sockaddr_in _sockAddr;
    FileDesc _fd;
};

#endif //ZEUS_ACCEPTOR_H
