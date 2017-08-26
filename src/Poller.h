//
// Created by comoon on 4/22/17.
//

#ifndef ZEUS_POLLER_H
#define ZEUS_POLLER_H

#include "Util.h"
#include <sys/epoll.h>
#include <memory>
#include <array>
#include <iostream>
#include <functional>

#define MAX_EPOLL_EVENTS 10000

struct Channel {
    int fd_;
    uint32_t events_;
    std::function<void()> fnOnRead_{nullptr};
    std::function<void()> fnOnHup_{nullptr};

    Channel(int f, uint32_t e) : fd_(f), events_(e) {}
};

typedef std::unique_ptr<Channel> channelPtr;


struct PollerBase : public Noncopyable {
    uint32_t active_;
    PollerBase() : active_(0) {}

    virtual void addChannel(channelPtr&& ch) = 0;
    virtual void removeChannel(int index) = 0;
    virtual void updateChannel(channelPtr&& ch) = 0;
    virtual int loopOnce(int waitMs) = 0;
    virtual ~PollerBase() {};
};

class Epoller : public PollerBase {

public:
    Epoller();
    ~Epoller();

    virtual void addChannel(channelPtr&& ch) override;
    virtual void removeChannel(int index) override;
    virtual void updateChannel(channelPtr&& ch) override ;
    virtual int loopOnce(int waitMs) override;

private:
    int fd_;
    struct epoll_event events_[MAX_EPOLL_EVENTS];
    std::array<channelPtr, MAX_EPOLL_EVENTS> channels_;
};


#endif //ZEUS_POLLER_H
