//
// Created by comoon on 8/26/17.
//

#ifndef ZEUS_HTTPSERVER_H
#define ZEUS_HTTPSERVER_H

#include "TcpServer.h"
#include "HttpProtocolParser.h"

class HttpServer : public TcpServer<HttpProtocolParser>{
public:
    HttpServer(std::shared_ptr<EventBase> ev) : TcpServer(ev){

    }
};


#endif //ZEUS_HTTPSERVER_H
