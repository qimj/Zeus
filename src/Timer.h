//
// Created by comoon on 4/26/17.
//

#ifndef ZEUS_TIMER_H
#define ZEUS_TIMER_H

#include "src/SmallHeap.h"
#include <functional>

typedef std::function<void()> Func;

struct Timer {
    bool isRepeat;
    Func callBack;
    uint64_t msInterval;
    uint64_t msToRun;

    Timer() {}
    Timer(uint64_t interval, uint64_t toRun, Func && f, bool repeat = false) :
            msInterval(interval), msToRun(toRun), callBack(std::move(f)), isRepeat(repeat){
        assert(msInterval % 10 == 0);
    }

    Timer(const Timer & t) : isRepeat(t.isRepeat), callBack(std::move(t.callBack)), msInterval(t.msInterval), msToRun(t.msToRun) {}
    Timer& operator=(const Timer & t) {
        isRepeat = t.isRepeat;
        callBack = std::move(t.callBack);
        msInterval = t.msInterval;
        msToRun = t.msToRun;
        return *this;
    }
    Timer(Timer && t) : isRepeat(t.isRepeat), callBack(std::move(t.callBack)), msInterval(t.msInterval), msToRun(t.msToRun) {
    }
    Timer& operator=(Timer && t) {
        isRepeat = t.isRepeat;
        callBack = std::move(t.callBack);
        msInterval = t.msInterval;
        msToRun = t.msToRun;
        return *this;
    }

    ~Timer(){}

    bool operator < (const Timer& t) const {
        return t.msToRun < msToRun;
    }
};

class TimerMg {
public:
    uint32_t ScheduleOnce(uint32_t ms) {

        for(auto &t : timers) {
            t.msToRun = t.msToRun - ms;

            if(t.msToRun <= 0)
                t.callBack();
        }

        while(timers.size()) {
            auto front = timers.front();
            if(front.msToRun <= 0) {
                if(front.isRepeat)
                    timers.push_back(Timer(front.msInterval, front.msInterval, std::move(front.callBack), front.isRepeat));

                timers.pop_front();
            }else{
                return front.msToRun;
            }
        }
    }

    bool Empty() { return timers.size() == 0;}
    bool Push(Timer && t) {
        timers.push_back(std::move(t));
    }

private:
    SmallHeap<Timer> timers;
};


#endif //ZEUS_TIMER_H
