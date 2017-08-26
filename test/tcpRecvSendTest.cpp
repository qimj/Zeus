//
// Created by comoon on 6/13/17.
//


#include <array>
#include <gtest/gtest.h>
#include <thread>
#include "src/TcpClient.h"
#include "src/ProtocolParser.h"

//case for test Tcp server recv and pack msg by splite one msg into several pieces
//and send several times
//./Z_test --gtest_filter=tcpsend.recv

using namespace std;

TEST(tcpsend, recv){
    TcpClient client(5660, "127.0.0.1");

    Protocol::MSG head;
    head.magic = MAGIC;
    head.protocolId = 1;

    const char * str{"hi"};
    head.dataLen = 3;

    client.Send((char*)&head, 12);
    client.Send(str, 3);
    this_thread::sleep_for(chrono::seconds{1});
    
    client.Send((char*)&head, 12);
    client.Send(str, 3);
    this_thread::sleep_for(chrono::seconds{1});

    client.Send((char*)&head, 12);
    client.Send(str, 3);
    this_thread::sleep_for(chrono::seconds{1});

//    client.Send((char*)&head, 2);
//    this_thread::sleep_for(chrono::seconds{1});
//    client.Send((char*)(&head + 2), 6);

//    char buf[7];
//    memcpy(buf, &head + 8, 4);
//    memcpy(buf + 4, str, 2);
//    buf[6] = '\0';
//
//    this_thread::sleep_for(chrono::seconds{1});
//    client.Send(buf, 7);
    
    this_thread::sleep_for(chrono::seconds{1000});
}