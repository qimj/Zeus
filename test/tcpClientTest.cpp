//
// Created by comoon on 5/21/17.
//

#include <gtest/gtest.h>
#include "src/TcpClient.h"
#include "src/ProtocolParser.h"

TEST(tcp, client){

    TcpClient client(5660, "127.0.0.1");
    Protocol::MSG head;
    head.magic = MAGIC;
    head.protocolId = 1;

    while(true) {

        std::string str;
        std::cin >> str;

        head.dataLen = str.length();
        IOBuf buf(sizeof(head) + head.dataLen);

        buf.Put((char*)&head, sizeof(head));
        buf.Put(str.c_str(), str.length());

        client.Send(buf);

        std::cout << "send one msg!" << std::endl;
    }
}