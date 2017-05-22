//
// Created by comoon on 4/22/17.
//

#include "Poller.h"
#include <unistd.h>
#include <cstring>
#include <iostream>


Epoller::Epoller(){
    fd_ = epoll_create(MAX_EPOLL_EVENTS);
    throw_system_error_on(fd_ == -1, "epoll_create");
    memset(events_, 0, MAX_EPOLL_EVENTS * sizeof(epoll_event));
}

Epoller::~Epoller(){
    ::close(fd_);
}

void Epoller::addChannel(channelPtr ch) {
    setFd_NonBlocking(ch->fd_);

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events_;
    ev.data.ptr = ch.get();

    int r = epoll_ctl(fd_, EPOLL_CTL_ADD, ch->fd_, &ev);
    throw_system_error_on(r == -1, "epoll_ctl");
    channels_[ch->fd_] = ch;
}

void Epoller::removeChannel(channelPtr ch) {
    channels_[ch->fd_] = nullptr;
    for(auto &i : events_) {
        if(ch.get() == i.data.ptr)
            i.data.ptr = nullptr;
    }
}

void Epoller::updateChannel(channelPtr ch) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events_;
    ev.data.ptr = ch.get();

    int r = epoll_ctl(fd_, EPOLL_CTL_MOD, ch->fd_, &ev);
    throw_system_error_on(r == -1, "epoll_ctl");
    channels_[ch->fd_] = ch;
}

void Epoller::loopOnce(int waitMs) {

    active_ = epoll_wait(fd_, events_, MAX_EPOLL_EVENTS, waitMs);
    throw_system_error_on(active_ == -1, "epoll_wait");

    for (auto i = 0; i < active_; ++i) {

        if(events_[i].data.ptr == nullptr)
            continue;

        if(events_[i].events & EPOLLIN) {
            auto f = (Channel*)events_[i].data.ptr;
            f->fnOnRead_();
        } else if(events_[i].events & (EPOLLERR | EPOLLHUP)) {

        } else {

        }
    }

}


