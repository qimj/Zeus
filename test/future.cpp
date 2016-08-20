//
// Created by paoli on 16/7/26.
//

#include <gtest/gtest.h>
#include <folly/futures/Future.h>

void foo(int x) {
    std::cout << " x is : " << x << std::endl;
}

TEST(future, folly_future) {

    folly::Promise<int> p;
    std::thread t1([&](){

        for(auto i = 0; i < 5; ++i) {
            std::cout << "-" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds{1});
        }

        p.setValue(13);
    });

    std::thread t2([&](){

        std::cout << " t2 start" << std::endl;
        folly::Future<int> f = p.getFuture();
        f.then(foo);
        std::cout << " t2 end" << std::endl;

    });

    t1.join();
    t2.join();
}
