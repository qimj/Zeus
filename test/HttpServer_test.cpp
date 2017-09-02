//
// Created by comoon on 8/26/17.
//

#include <gtest/gtest.h>
#include "src/HttpServer.h"

TEST(httpserver, test) {

    auto mainLoop = std::make_shared<EventLoop>();

    HttpServer sv(mainLoop);
    sv.listen(8080, "127.0.0.1");

    sv.onNewConnect([](const std::shared_ptr<Connection>& con){
        LOG_DEBUG << "a new connection is coming!" << endl;
    });

    HTTP_ROUTE(sv, "/test/test").methods("POST")(
            [](const HttpRequest& req, HttpResponse& rep){
                LOG_DEBUG << "url : " << req._url << endl;
                LOG_DEBUG << "body : " << req._body << endl;
                for(auto const &i : req._heads)
                    LOG_DEBUG << i.first << ":" << i.second << endl;
            }
    );

    HTTP_ROUTE(sv, "/greet").methods("GET")(
            [](const HttpRequest& req, HttpResponse& rep){
                rep._body = "hello, world";
            }
    );

    mainLoop->loop_forever();
}