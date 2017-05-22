//
// Created by comoon on 4/7/17.
//

#ifndef ASYN_TEST_BLOCKINGQUEUE_H
#define ASYN_TEST_BLOCKINGQUEUE_H

#include <list>
#include <mutex>
#include <condition_variable>

template <typename T>
class BlockingQueue {

public:

    BlockingQueue(int maxSize) : _maxSize(maxSize) {}

    bool Put(const T& x){ return Add(x); }
    bool Put(T&& x) { return Add(std::forward<T>(x)); }

    T Take() {
        std::unique_lock<std::mutex> locker(_mutex);
        _noEmpty.wait(locker, [this]{ return !_queue.empty(); });
        auto x = _queue.front();
        _queue.pop_front();
        return std::move(x);
    }

    int Size() {
        std::lock_guard<std::mutex> locker(_mutex);
        return _queue.size();
    }

    bool IsEmpty() {
        std::lock_guard<std::mutex> locker(_mutex);
        return _queue.empty();
    }

    bool IsFull() {
        std::lock_guard<std::mutex> locker(_mutex);
        return _queue.size() == _maxSize;
    }

private:

    bool Add(T&& x) {
        std::lock_guard<std::mutex> locker(_mutex);
        if(_queue.size() >= _maxSize)
            return false;

        _queue.emplace_back(std::forward<T>(x));
        _noEmpty.notify_one();

        return true;
    }

    int _maxSize;
    std::list<T> _queue;
    std::mutex _mutex;
    std::condition_variable _noEmpty;

};

#endif //ASYN_TEST_BLOCKINGQUEUE_H
