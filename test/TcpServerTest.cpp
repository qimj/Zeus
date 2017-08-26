//
// Created by comoon on 5/20/17.
//

#include <gtest/gtest.h>
#include "src/TcpServer.h"


TEST(tcp, server) {

    TcpServer<TcpProtocolParser> sv;
    sv.listen(5660, "127.0.0.1");

    sv.onNewConnect([](const std::shared_ptr<Connection>& con){
        cout << " a new connection is coming!" << endl;
    });

    sv.start();
}