//
// Created by comoon on 5/17/17.
//

#include <gtest/gtest.h>
#include "src/future/future.hh"

using namespace std;

TEST(future3, test) {

    promise<int> p;

    p.get_future().then([](auto i){
        cout << "hehe" << endl;
        this_thread::sleep_for(chrono::seconds(3));
    }).then([](){
        cout << "after hehe" << endl;
    }).then([](){
        cout << "after after hehe" << endl;
    });

    cout << "set" << endl;
    p.set_value(10);

    make_ready_future<int, int>(2,3).then([](auto a, auto b){
        cout << "a is " << a << " b is " << b << endl;
    });

    engine().run();
}