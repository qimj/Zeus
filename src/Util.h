//
// Created by comoon on 4/22/17.
//

#ifndef ZEUS_UTIL_H
#define ZEUS_UTIL_H

#include <assert.h>
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

inline unsigned char FromHex(unsigned char x) {
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(!"must 0～9　a～z!");
    return y;
}

inline std::string URLDecode(const char* str,size_t len) {
    std::string strTemp = "";
    size_t length = len;
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+') strTemp += ' ';
        else if (str[i] == '%') {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high*16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}


#endif //ZEUS_UTIL_H
