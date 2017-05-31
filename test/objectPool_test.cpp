//
// Created by comoon on 5/25/17.
//


#include "src/ObjectPool.h"
#include <gtest/gtest.h>
#include <memory>

struct MyObj : public PoolObject<MyObj>{

};

TEST(objectPool, test) {

    MyObj * a1 = new MyObj();
    MyObj * a2 = new MyObj();

    delete a1;
    delete a2;

    MyObj * a3 = new MyObj();
    MyObj * a4 = new MyObj();

    std::shared_ptr<MyObj> a5(new MyObj());

    a5.reset();

    std::shared_ptr<MyObj> a6(new MyObj());
}