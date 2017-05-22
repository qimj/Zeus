//
// Created by comoon on 5/19/17.
//

#include "src/ThreadPool.h"
#include <gtest/gtest.h>

TEST(thread_pool, test) {

    ThreadPool pool(4);

    pool.indexPush([](){
        std::cout << "i run in " << std::this_thread::get_id() << std::endl;
    }, 0);

    pool.indexPush([](){
        std::cout << "i run in " << std::this_thread::get_id() << std::endl;
    }, 1);

    pool.indexPush([](){
        std::cout << "i run in " << std::this_thread::get_id() << std::endl;
    }, 2);

    pool.indexPush([](){
        std::cout << "i run in " << std::this_thread::get_id() << std::endl;
    }, 3);

}