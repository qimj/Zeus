//
// Created by comoon on 4/26/17.
//

#ifndef ZEUS_SMALLHEAP_H
#define ZEUS_SMALLHEAP_H

#include <deque>
#include <algorithm>
#include <assert.h>

using namespace std;

template <typename T>
class SmallHeap {

public:
    SmallHeap(){ make_heap(_que.begin(), _que.end()); }
    ~SmallHeap() {}

    auto begin() { return _que.begin(); }
    auto end() { return _que.end(); }
    T front() { return _que.front(); }
    T& at(uint32_t n) { assert(n < _que.size()); return _que.at(n); }
    T& operator [] (uint32_t n) { assert(n < _que.size()); return _que[n]; }
    uint32_t size() { return _que.size(); }

    void pop_front() {
        pop_heap (_que.begin(),_que.end());
        _que.pop_back();
    }

    void push_back(T && t) {
        _que.emplace_back(t);
        push_heap(_que.begin(), _que.end());
    }

private:
    deque<T> _que;
};

#endif //ZEUS_SMALLHEAP_H
