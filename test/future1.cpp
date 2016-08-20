//
// Created by paoli on 16/8/13.
//

#include <gtest/gtest.h>
#include <folly/futures/Future.h>

class MyType {

public:
    void operator() () {
        std::cout << "hehe" << std::endl;
    }
};

int put(){
    std::cout << "put" << std::endl;
    return 3;
}

TEST(future1, future1){

    folly::Promise<MyType> me;
    auto fu = me.getFuture();

    fu.then([&](){
        std::cout << "type has finished!" << std::endl;
        auto f = fu.get();
        f();
    }).then([](){
        return put();
    }).then([](int x){
        std::cout << "x is : " << x << std::endl;
    });

    std::cout << "set value" << std::endl;
    me.setValue(std::move(MyType()));
}
