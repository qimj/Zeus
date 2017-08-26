//
// Created by comoon on 6/14/17.
//

#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>

using namespace std;

TEST(lambda, test) {

    shared_ptr<int> ptr(new int(1));

    thread t1([ptr](){
        cout << "before sleep : " << ptr.use_count() << endl;

        this_thread::sleep_for(chrono::seconds{3});

        cout << "after sleep : " << ptr.use_count() << endl;
    });

    this_thread::sleep_for(chrono::seconds{1});

    auto x = [ptr](){};

    cout << "cnt outside : " << ptr.use_count() << endl;

    ptr.reset();

    cout << " after reset : " << ptr.use_count() << endl;

    t1.join();
}