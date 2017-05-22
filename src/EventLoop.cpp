//
// Created by comoon on 4/22/17.
//

#include "EventLoop.h"

EventLoop::EventLoop() {
    _timerMg = std::make_unique<TimerMg>();
    _poller = std::make_unique<Epoller>();
}

void EventLoop::add_timer(std::shared_ptr<Timer> &) {

}

void EventLoop::loop_once(uint32_t ms) {
    _poller->loopOnce(ms);
}

void EventLoop::loop_forever() {

    while(_running) {

        _poller->loopOnce(100);

    }

}
