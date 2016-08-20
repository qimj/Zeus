//
// Created by paoli on 16/8/11.
//

#include <gtest/gtest.h>
#include <folly/futures/Future.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>


void putThread(){

    std::cout << std::this_thread::get_id() << std::endl;

}

TEST(future2, future2){

    wangle::CPUThreadPoolExecutor firstPool(2);
    wangle::CPUThreadPoolExecutor secondPool(2);

    int time = 10;
    while(time --) {

        folly::via(&firstPool).then(putThread);
        folly::via(&secondPool).then(putThread);

        std::this_thread::sleep_for(std::chrono::seconds{1});
    }

}
