//
// Created by comoon on 5/21/17.
//

#include <gtest/gtest.h>
#include "src/TcpClient.h"
#include "src/ProtocolParser.h"


//--gtest_filter=tcpClient.test
// to executor a target gtest case
TEST(tcpClient, test){

    TcpClient client(5660, "127.0.0.1");
    Protocol::MSG head;
    head.magic = MAGIC;
    head.protocolId = 1;

    while(true) {

        std::string str;
        std::cin >> str;

        head.dataLen = str.length() + 1;
        IOBuf buf(sizeof(head) + head.dataLen);

        buf.Put((char*)&head, sizeof(head));
        buf.Put(str.c_str(), str.length() + 1);

        client.Send(buf);

        std::cout << "send one msg : " << " " << str.length() << std::endl;
        buf.Print(sizeof(head));
    }
}