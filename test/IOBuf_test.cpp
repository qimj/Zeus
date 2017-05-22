//
// Created by comoon on 5/20/17.
//

#include <gtest/gtest.h>
#include "src/IOBuf.h"

TEST(IOBuf, test) {

    const char * hi = "hi";
    const char * hello = "hello";
    const char * nice = "nice to meet u";

    IOBuf buf(strlen(hi) + strlen(hello) + strlen(nice));

    buf.Put(hi, strlen(hi));
    buf.Put(hello, strlen(hello));
    buf.Put(nice, strlen(nice));

    std::cout << buf.Get() << std::endl;
}