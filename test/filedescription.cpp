//
// Created by paoli on 16/8/17.
//


#include <gtest/gtest.h>
#include "../src/common/FileDesc.h"

TEST(fd, filedescription) {

    auto myFile = FileDesc::open("./myFile", O_CREAT | O_RDWR);

    std::string buf("hello, world");
    myFile.write(buf.c_str(), buf.size());

    myFile.close();
}
