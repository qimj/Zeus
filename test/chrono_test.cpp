//
// Created by comoon on 6/12/17.
//

#include <gtest/gtest.h>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

TEST(chrono, test) {

    steady_clock::time_point t1 = steady_clock::now();

    std::this_thread::sleep_for(chrono::seconds{3});

    steady_clock::time_point t2 = steady_clock::now();

    auto eslaped = duration_cast<chrono::milliseconds>(t2 - t1);

    cout << "it's eslaped " << eslaped.count() << " Mseconds" << endl;
}
