//
// Created by comoon on 4/26/17.
//

#include <iostream>
#include <gtest/gtest.h>
#include "src/Timer.h"
#include "src/SmallHeap.h"


using namespace std;

TEST(timer, test) {

    SmallHeap<Timer> heap;

    heap.push_back(Timer(10, 9, [](){}));
    heap.push_back(Timer(10, 3, [](){}));
    heap.push_back(Timer(10, 11, [](){}));
    heap.push_back(Timer(10, 6, [](){}));
    heap.push_back(Timer(10, 5, [](){}));

    cout << "smallest : " << heap.front().msToRun << endl;

    heap.pop_front();

    cout << "smallest : " << heap.front().msToRun << endl;

    heap.pop_front();

    cout << "smallest : " << heap.front().msToRun << endl;

    heap.pop_front();

    cout << "smallest : " << heap.front().msToRun << endl;

    heap.pop_front();

    cout << "smallest : " << heap.front().msToRun << endl;

    heap.at(0).msToRun = 100;

    cout << "smallest : " << heap.front().msToRun << endl;

    cout << "size : " << heap.size() << endl;

    for (auto &i : heap)
        i.msToRun = 123;

    cout << heap.at(0).msToRun << endl;
}