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

#include "Log.h"
#include "Connection.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include "TcpProtocolParser.h"

#define LISTENQ 10000
#define MAX_TCP_CONNECTIONS 100000

extern void throw_system_error_on(bool condition, const char* what_arg);

template <typename TcpParser>
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
    TcpServer() : TcpServer(std::make_shared<EventLoop>()){
    }

    TcpServer(std::shared_ptr<EventBase> ev) {
        _evloop = ev;
        _ioThreadPool = std::make_shared<ThreadPool>(4);
    }

    ~TcpServer() {}

    void listen(uint32_t port, const char * serverAddr = nullptr) {
        _listener = std::make_unique<Listener>(port, serverAddr);
        channelPtr c = std::make_unique<Channel>(_listener->_fd, EPOLLIN | EPOLLET);
        c->fnOnRead_ = [this]{ accept(); };
        _evloop->_poller->addChannel(std::move(c));
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
                LOG_DEBUG << "Accept in : " << newCon->_fd << endl;

            //create parser for new connection and it has the same life with connection
            static_assert(std::is_base_of<ProtocolParser, TcpParser>::value);

            auto parser = std::make_shared<TcpParser>();
            channelPtr c = std::make_unique<Channel>(newCon->_fd, EPOLLIN | EPOLLRDHUP | EPOLLET);

            c->fnOnRead_ = [this, parser, fd = newCon->_fd]() {

                _ioThreadPool->indexPush([this, fd, parser]() {

                    parser->OnMsg([](const std::shared_ptr<IOBuf> &&buf){
                        static_pointer_cast<ConstIOBuf>(buf)->Print();
                    });

                    auto res = parser->ParseMsg(fd);
                    if(!res)
                        dropConnect(fd);
                }, fd);
            };

            c->fnOnHup_ = [this, fd = newCon->_fd](){
                _ioThreadPool->indexPush([this, fd]() {
                    dropConnect(fd);
                }, fd);
            };

            _connections[newCon->_fd] = newCon;
            _evloop->_poller->addChannel(std::move(c));

            if (_fnOnConnection)
                _fnOnConnection(newCon);

        }while(true);
    }

    void dropConnect(int fd){
        if(_connections[fd]){
            LOG_DEBUG << "Drop connect : " << fd << endl;

            _evloop->_poller->removeChannel(fd);

            _connections[fd].reset();
            _connections[fd] = nullptr;
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
