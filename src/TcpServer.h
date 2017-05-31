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

#include "Log.h"
#include "Connection.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include "ProtocolParser.h"

#define LISTENQ 10000
#define MAX_TCP_CONNECTIONS 100000

extern void throw_system_error_on(bool condition, const char* what_arg);

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

        LOG_DEBUG << "Server listend on : " << port <<  " " << serverAddr << endl;
    }

    void start() {
        _evloop->loop_forever();
    }

    typedef std::function<void(const std::shared_ptr<Connection>&)> FnOnCon;
    void onNewConnect(const FnOnCon& fnOnCon) { _fnOnConnection = fnOnCon; }

private:

    void accept() {
        do {
            connectionPtr newCon(new Connection());
            newCon->_fd = ::accept(_listener->_fd, (sockaddr *) &newCon->_sockAddr, &newCon->_addr_len);
            if(newCon->_fd == -1)
                break;
            else
                LOG_DEBUG << "accept in : " << newCon->_fd << endl;

            newCon->setIp();
            channelPtr c = std::make_shared<Channel>(newCon->_fd, EPOLLIN | EPOLLET);
            c->fnOnRead_ = [this, fd = newCon->_fd]() {

                _ioThreadPool->indexPush([this, fd]() {
                    auto parser = std::make_shared<ProtocolParser>();
                    auto res = parser->ParseMsg(fd);

                    if(!res)
                        dropConnect(fd);

                    parser->OnMsg([](const ProtocolParser::_REQ & fnOnMsg){
                        LOG_DEBUG << "wrapped a msg!" << endl;
                    });

                }, fd);
            };

            _connections[newCon->_fd] = newCon;
            _evloop->_poller->addChannel(c);

            if (_fnOnConnection)
                _fnOnConnection(newCon);

        }while(true);
    }

    void dropConnect(int fd){
        if(_connections[fd]){

            LOG_DEBUG << "drop connect : " << _connections[fd]->_ip << endl;

            _connections[fd].reset();
            _connections[fd] = nullptr;

            _evloop->_poller->removeChannel(fd);
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
