//
// Created by comoon on 4/22/17.
//

#include "Poller.h"
#include <chrono>
#include <unistd.h>
#include <cstring>

Epoller::Epoller(){
    fd_ = epoll_create(MAX_EPOLL_EVENTS);
    throw_system_error_on(fd_ == -1, "epoll_create");
    memset(events_, 0, MAX_EPOLL_EVENTS * sizeof(epoll_event));
}

Epoller::~Epoller(){
    ::close(fd_);
}

void Epoller::addChannel(channelPtr&& ch) {
    setFd_NonBlocking(ch->fd_);

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events_;
    ev.data.ptr = ch.get();

    int r = epoll_ctl(fd_, EPOLL_CTL_ADD, ch->fd_, &ev);
    throw_system_error_on(r == -1, "epoll_ctl");
    channels_[ch->fd_] = std::move(ch);
}

void Epoller::removeChannel(int index) {
    channels_[index].reset();
    channels_[index] = nullptr;
}

void Epoller::updateChannel(channelPtr&& ch) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events_;
    ev.data.ptr = ch.get();

    int r = epoll_ctl(fd_, EPOLL_CTL_MOD, ch->fd_, &ev);
    throw_system_error_on(r == -1, "epoll_ctl");
    channels_[ch->fd_] = std::move(ch);
}

int Epoller::loopOnce(int waitMs) {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    active_ = epoll_wait(fd_, events_, MAX_EPOLL_EVENTS, waitMs);
    throw_system_error_on(active_ == -1, "epoll_wait");

    for (auto i = 0; i < active_; ++i) {

        if(events_[i].data.ptr == nullptr)
            continue;

        auto f = (Channel*)events_[i].data.ptr;
        if (events_[i].events & EPOLLRDHUP)
            f->fnOnHup_();
        else if(events_[i].events & EPOLLIN)
            f->fnOnRead_();
        else if(events_[i].events & EPOLLERR) {

        } else if (events_[i].events & EPOLLOUT){

        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}