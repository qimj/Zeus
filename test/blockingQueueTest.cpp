//
// Created by comoon on 4/7/17.
//

#include <gtest/gtest.h>
#include <thread>
#include "src/BlockingQueue.h"

using namespace std;

TEST(blocking, queue) {

    BlockingQueue<int> queue(10);
    queue.Put(1);
    queue.Put(2);

    std::thread t0([&](){
        while(true) {
            auto x = queue.Take();
            cout << this_thread::get_id() << " consumed : " << x << endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });

    std::thread t1([&](){
        while(true) {
            auto x = queue.Take();
            cout << this_thread::get_id() << " consumed : " << x << endl;
            std::this_thread::sleep_for(std::chrono::seconds(4));
        }
    });

    std::thread t2([&](){
        while(true) {
            queue.Put(4);
            cout << "produced " << this_thread::get_id() << endl;
            std::this_thread::sleep_for(std::chrono::seconds(6));
        }
    });

    std::thread t3([&](){
        while(true) {
            queue.Put(5);
            cout << "produced " << this_thread::get_id() << endl;
            std::this_thread::sleep_for(std::chrono::seconds(6));
        }
    });

    t0.join();
    t1.join();
    t2.join();
    t3.join();
}