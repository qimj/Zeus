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

    mainLoop->loop_forever();
}