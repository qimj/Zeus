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

class TcpClient {

public:
    TcpClient(uint32_t port, const char * ip = nullptr) {
        _sockFd = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&_addr, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(port);

        if(ip)
            inet_pton(AF_INET, ip, &_addr.sin_addr);
        else
            inet_pton(AF_INET, "127.0.0.1", &_addr.sin_addr);

        int r = connect(_sockFd, (struct sockaddr *)&_addr, sizeof(_addr));
        if(-1 == r) {
            std::cout << "connect " << ip << " error" << std::endl;
        }
    }

    TcpClient() { ::close(_sockFd); }
    bool Send(std::shared_ptr<IOBuf>& buf) {
        send(_sockFd, buf->Get().get(), buf->Len(), 0);
    }

    bool Send(IOBuf & buf) {
        send(_sockFd, buf.Get().get(), buf.Len(), 0);
    }
protected:
    struct sockaddr_in _addr;
    int _sockFd;
};

#endif //ZEUS_TCPCLIENT_H
