//
// Created by comoon on 5/19/17.
//

#ifndef ASYN_TEST_THREADPOOL_H
#define ASYN_TEST_THREADPOOL_H

#include "BlockingQueue.h"
#include <thread>
#include <cassert>
#include <atomic>
#include <vector>

struct Task {
    std::chrono::steady_clock::time_point enqueueTime;
    std::chrono::milliseconds runTime;

    Task() { enqueueTime = std::chrono::steady_clock::now(); }
    virtual ~Task() {}
    virtual void run() = 0;
};

template <typename Func>
struct LambdaTask final : public Task {
    Func _f;
    LambdaTask(const Func& func) : _f(func) {}
    LambdaTask(Func&& func) : _f(std::move(func)) {}
    virtual void run() override { _f(); }
};

struct ThreadObj {

    static int _id_seq;
    int _id;
    std::atomic<bool> _stop {false};
    std::thread _handle;
    BlockingQueue<std::shared_ptr<Task>> _taskQueue;

    ThreadObj() : _taskQueue(1024), _id(_id_seq ++){
        _handle = std::thread([this](){
            while(!_stop) {
                auto f = _taskQueue.Take();
                f->run();
            }
        });
    }

    ~ThreadObj() {
        _stop = true;
        _handle.join();
    }

    template <typename Func>
    void submit(Func && f){
        _taskQueue.Put(std::make_shared<LambdaTask<Func>>(f));
    };

    void stop() { _stop = true; }
    int getId() { return _id; }
};

class ThreadPool {

public:
    ThreadPool (uint8_t size = 4) : _threadCnt(size) {
        for (auto i = 0; i < size; ++i) {
            _threads.push_back(std::make_shared<ThreadObj>());
        }
    }

    ~ThreadPool() {
        for (auto &i : _threads)
            i->stop();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool&operator = (const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool&operator = (ThreadPool &&) = delete;

    template <typename Func>
    void indexPush(Func&& f, uint8_t index){
        assert(index < _threadCnt);
        _threads[index]->submit(std::forward<Func>(f));
    }

private:
    std::vector<std::shared_ptr<ThreadObj>> _threads;
    uint8_t _threadCnt;
};

#endif //ASYN_TEST_THREADPOOL_H