//
// Created by comoon on 4/22/17.
//

#ifndef ZEUS_UTIL_H
#define ZEUS_UTIL_H

#include <fcntl.h>
#include <system_error>

inline void throw_system_error_on(bool condition, const char* what_arg) {
    if (condition) {
        throw std::system_error(errno, std::system_category(), what_arg);
    }
}

struct Noncopyable {
    Noncopyable() {};
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

inline void setFd_NonBlocking (int fd) {
    int flag;
    flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}


#endif //ZEUS_UTIL_H
