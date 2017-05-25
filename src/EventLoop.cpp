//
// Created by comoon on 4/22/17.
//

#include "src/Log.h"
#include "EventLoop.h"

EventLoop::EventLoop() {
    _timerMg = std::make_unique<TimerMg>();
    _poller = std::make_unique<Epoller>();
}

void EventLoop::add_timer(Timer && t) {
    _timerMg->Push(std::move(t));
}

void EventLoop::loop_once(uint32_t ms) {
    _poller->loopOnce(ms);
    LOG_DEBUG << "loop ms : " << ms << endl;
}

void EventLoop::loop_forever() {

    while(_running) {

        if(!_timerMg->Empty()){
            _lastLoopMs = _timerMg->ScheduleOnce(_lastLoopMs);
        }

        _poller->loopOnce(_lastLoopMs);
    }

}
