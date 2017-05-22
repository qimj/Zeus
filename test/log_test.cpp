//
// Created by comoon on 4/26/17.
//

#include <sstream>
#include <gtest/gtest.h>
#include "src/Log.h"

using namespace std;

TEST(log, test) {

    LOG_INFO << "This is " << " just" << 123 << " a " << true << " test" << endl;

    LOG_WARN << "enn en ?" << false << endl;

    LOG_DEBUG << "heihei" << 124345 << endl;
}