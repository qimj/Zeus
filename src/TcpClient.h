//
// Created by comoon on 5/21/17.
//

#ifndef ZEUS_TCPCLIENT_H
#define ZEUS_TCPCLIENT_H

#include "IOBuf.h"
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

extern void throw_system_error_on(bool condition, const char* what_arg);

class TcpClient {

public:
    TcpClient(uint32_t port, const char * ip = nullptr) {
        _sockFd = socket(AF_INET, SOCK_STREAM, 0);
        throw_system_error_on(-1 == _sockFd, "socket");
        bzero(&_addr, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(port);

        if(ip)
            inet_pton(AF_INET, ip, &_addr.sin_addr);
        else
            inet_pton(AF_INET, "127.0.0.1", &_addr.sin_addr);

        int r = connect(_sockFd, (struct sockaddr *)&_addr, sizeof(_addr));
        throw_system_error_on(-1 == r, "connect");
    }

    ~TcpClient() { ::close(_sockFd); }
    bool Send(std::shared_ptr<IOBuf>& buf) {
        int r = send(_sockFd, buf->Get().get(), buf->Len(), 0);
        throw_system_error_on(-1 == r, "send");
    }

    bool Send(IOBuf & buf) {
        int r = send(_sockFd, buf.Get().get(), buf.Len(), 0);
        throw_system_error_on(-1 == r, "send");
    }
protected:
    struct sockaddr_in _addr;
    int _sockFd;
};

#endif //ZEUS_TCPCLIENT_H
