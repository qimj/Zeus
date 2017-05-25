//
// Created by comoon on 5/21/17.
//

#include <array>
#include <gtest/gtest.h>
#include <thread>
#include "src/TcpClient.h"
#include "src/ProtocolParser.h"


//--gtest_filter=tcpClient.test
// to executor a target gtest case
//watch ss -s  check tcp connections
TEST(tcpClient, test){


    std::array<std::shared_ptr<TcpClient>, 1000> clients;
    for(auto i = 0; i < clients.size(); ++i)
        clients[i] = std::make_shared<TcpClient>(5660, "127.0.0.1");

    std::this_thread::sleep_for(std::chrono::seconds(10000));

//    Protocol::MSG head;
//    head.magic = MAGIC;
//    head.protocolId = 1;

//    while(true) {
//
//        std::string str;
//        std::cin >> str;
//
//        head.dataLen = str.length() + 1;
//        IOBuf buf(sizeof(head) + head.dataLen);
//
//        buf.Put((char*)&head, sizeof(head));
//        buf.Put(str.c_str(), str.length() + 1);
//
//        client.Send(buf);
//
//        std::cout << "send one msg : " << " " << str.length() << std::endl;
//        buf.Print(sizeof(head));
//    }
}