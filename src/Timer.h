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
            msInterval(interval), msToRun(toRun), callBack(f), isRepeat(repeat){}

    bool operator < (const Timer& t) const {
        return t.msToRun < msToRun;
    }
};

class TimerMg {

public:

    void ScheduleOnce(uint32_t ms) {

    }

private:

    SmallHeap<Timer> timers;
};


#endif //ZEUS_TIMER_H
