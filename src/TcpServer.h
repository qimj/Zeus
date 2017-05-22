//
// Created by comoon on 5/19/17.
//

#ifndef ZEUS_TCPSERVER_H
#define ZEUS_TCPSERVER_H

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <iostream>
#include "EventLoop.h"
#include "ThreadPool.h"
#include "ProtocolParser.h"

#define LISTENQ 1000
#define MAX_TCP_CONNECTIONS 10000

extern void throw_system_error_on(bool condition, const char* what_arg);

struct Connection {
    int _fd;
    struct sockaddr_in _sockAddr;
    socklen_t _addr_len{sizeof(_sockAddr)};

    Connection(){}
    ~Connection(){ ::close(_fd); }
};

typedef std::shared_ptr<Connection> connectionPtr;


class TcpServer {

    struct Listener {
        int _fd;
        struct sockaddr_in _sockAddr;

        Listener(uint32_t port, const char * serverAddr) {
            _fd = socket(AF_INET, SOCK_STREAM, 0);
            throw_system_error_on(-1 == _fd, "socket");
            bzero(&_sockAddr, sizeof(_sockAddr));
            _sockAddr.sin_family = AF_INET;
            _sockAddr.sin_port = htons(port);

            if(serverAddr == nullptr)
                _sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            else
                inet_pton(AF_INET, serverAddr, &_sockAddr.sin_addr);
            int r = bind(_fd, (struct sockaddr *)&_sockAddr, sizeof(_sockAddr));
            throw_system_error_on(-1 == r, "bind");
        }
        void listen() {
            int r = ::listen(_fd, LISTENQ);
            throw_system_error_on(-1 == r, "listen");
        }
        ~Listener() { ::close(_fd); }
    };

public:
    TcpServer() {
        _evloop = std::make_shared<EventLoop>();
//        _ioThreadPool = std::make_shared<ThreadPool>(2);
    }

    ~TcpServer() {}

    void listen(uint32_t port, const char * serverAddr = nullptr) {
        _listener = std::make_unique<Listener>(port, serverAddr);
        channelPtr c = std::make_shared<Channel>(_listener->_fd, EPOLLIN | EPOLLET);
        c->fnOnRead_ = [this]{ accept(); };
        _evloop->_poller->addChannel(c);
        _listener->listen();
    }

    void start() {
        _evloop->loop_forever();
    }

    typedef std::function<void(const std::shared_ptr<Connection>&)> FnOnCon;
    void onNewConnect(const FnOnCon& fnOnCon) { _fnOnConnection = fnOnCon; }

private:

    void accept() {
        for(connectionPtr newCon = std::make_shared<Connection>(); (newCon->_fd = ::accept(_listener->_fd, (sockaddr *)&newCon->_sockAddr, &newCon->_addr_len)) >= 0;) {

            channelPtr c = std::make_shared<Channel>(newCon->_fd, EPOLLIN | EPOLLET);
            c->fnOnRead_ = [c](){
                auto parser = std::make_shared<ProtocolParser>();
                parser->ParseMsg(c->fd_);

                parser->OnMsg([](const ProtocolParser::_REQ & fnOnMsg){
                    cout << "wrapped a msg!" << endl;
                });
            };

            _connections[newCon->_fd] = newCon;
            _evloop->_poller->addChannel(c);

            if(_fnOnConnection)
                _fnOnConnection(newCon);
        }
    }

private:
    std::shared_ptr<EventBase> _evloop;
    std::array<std::shared_ptr<Connection>, MAX_TCP_CONNECTIONS> _connections;
    std::shared_ptr<ThreadPool> _ioThreadPool;
    FnOnCon _fnOnConnection {nullptr};
    std::unique_ptr<Listener> _listener;

};


#endif //ZEUS_TCPSERVER_H
