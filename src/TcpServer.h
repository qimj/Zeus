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

#define LISTENQ 10000
#define MAX_TCP_CONNECTIONS 100000

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

            //to avoid "Address already in use" error
            int opt = 1;
            throw_system_error_on(-1 == setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "setsockopt");

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
        _ioThreadPool = std::make_shared<ThreadPool>(4);
    }

    TcpServer(std::shared_ptr<EventBase> ev) {
        _evloop = ev;
        _ioThreadPool = std::make_shared<ThreadPool>(4);
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
        do {
            connectionPtr newCon = std::make_shared<Connection>();
            newCon->_fd = ::accept(_listener->_fd, (sockaddr *) &newCon->_sockAddr, &newCon->_addr_len);
            if(newCon->_fd == -1)
                break;

            channelPtr c = std::make_shared<Channel>(newCon->_fd, EPOLLIN | EPOLLET);
            c->fnOnRead_ = [this, newCon]() {
                _ioThreadPool->indexPush([this, newCon]() {
                    auto parser = std::make_shared<ProtocolParser>();
                    auto res = parser->ParseMsg(newCon->_fd);

                    if(!res)
                        this->dropConnect(newCon);

                    parser->OnMsg([](const ProtocolParser::_REQ & fnOnMsg){
                        cout << "wrapped a msg!" << endl;
                    });

                }, newCon->_fd);
            };

            _connections[newCon->_fd] = newCon;
            _evloop->_poller->addChannel(c);

            if (_fnOnConnection)
                _fnOnConnection(newCon);

        }while(true);
    }

    void dropConnect(connectionPtr con){
        if(_connections[con->_fd])
            _connections[con->_fd] = nullptr;
    }

private:
    std::shared_ptr<EventBase> _evloop;
    std::array<std::shared_ptr<Connection>, MAX_TCP_CONNECTIONS> _connections;
    std::shared_ptr<ThreadPool> _ioThreadPool;
    FnOnCon _fnOnConnection {nullptr};
    std::unique_ptr<Listener> _listener;
};


#endif //ZEUS_TCPSERVER_H
