//
// Created by comoon on 5/20/17.
//

#include <gtest/gtest.h>
#include "src/IOBuf.h"

TEST(IOBuf, test) {

    const char * hi = "hi\n";
    const char * hello = "hello\n";
    const char * nice = "nice to meet u";

    ConstIOBuf buf(strlen(hi) + strlen(hello) + strlen(nice) + 1);

    buf.Put(hi, strlen(hi));
    buf.Put(hello, strlen(hello));
    buf.Put(nice, strlen(nice));
    buf.Put("\0", 1);

    std::cout << buf.Get() << std::endl;

    DynamicIOBuf dbuf;
    dbuf.Put(hi, strlen(hi));
    dbuf.Put(hello, strlen(hello));
    dbuf.Put(nice, strlen(nice));
    dbuf.Put("\0", 1);

    std::cout << dbuf.getHead() << std::endl;
    dbuf.Print();
}