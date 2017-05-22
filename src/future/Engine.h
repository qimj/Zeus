//
// Created by comoon on 5/4/17.
//

#ifndef FUTURETEST_ENGINE_H
#define FUTURETEST_ENGINE_H

#include <iostream>
#include <array>
#include <thread>
#include "task.hh"

class Engine {

public:
    void add_task(std::unique_ptr<task>&& t) { _pending_tasks[index] = std::move(t); index++; }
    void run(){
        for(; tail != index; tail ++) {
            std::cout << tail << " " << index << std::endl;
            _pending_tasks[tail].get()->run();
        }
    }

private:
    std::array<std::unique_ptr<task>, 100> _pending_tasks;
    static int index;
    static int tail;
};

static Engine eg;
Engine & engine() { return eg; }

int Engine::index = 0;
int Engine::tail = 0;



#endif //FUTURETEST_ENGINE_H
