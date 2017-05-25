//
// Created by comoon on 4/26/17.
//

#include <iostream>
#include <gtest/gtest.h>
#include "src/EventLoop.h"


using namespace std;

TEST(timer, test) {

    auto mainLoop = std::make_shared<EventLoop>();

    mainLoop->add_timer(Timer(3000, 0, [](){
        std::cout << "1" << std::endl;
    }, true));

    mainLoop->add_timer(Timer(2000, 1, [](){
        std::cout << "2" << std::endl;
    }, true));

    mainLoop->add_timer(Timer(6000, 2, [](){
        std::cout << "3" << std::endl;
    }, true));


    mainLoop->loop_forever();
}