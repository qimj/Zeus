//
// Created by comoon on 4/22/17.
//

#ifndef ZEUS_EVENTLOOP_H
#define ZEUS_EVENTLOOP_H

#include "Timer.h"
#include "Poller.h"
#include <atomic>
#include <stdint-gcc.h>
#include <memory>

class Timer;
class TimerMg;
class PollerBase;

class EventBase {
public:
    virtual ~EventBase() {};
    virtual void add_timer(Timer &&) = 0;
    virtual void loop_once(uint32_t ms) = 0;
    virtual void loop_forever() = 0;
    bool isRunning() { return _running; }

    std::atomic<bool> _running {true};
    std::unique_ptr<TimerMg> _timerMg {nullptr};
    std::unique_ptr<PollerBase> _poller {nullptr};
    uint32_t _lastLoopMs{0};
};

class EventLoop : public EventBase{
public:
    EventLoop();
    virtual void add_timer(Timer &&) override ;
    virtual void loop_once(uint32_t ms) override;
    virtual void loop_forever() override;
};


#endif //ZEUS_EVENTLOOP_H
