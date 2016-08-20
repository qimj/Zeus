//
// Created by paoli on 16/8/17.
//

#ifndef ZEUS_FILEDESC_H
#define ZEUS_FILEDESC_H

#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <system_error>
#include <string>
#include <sys/socket.h>
//#include <sys/epoll.h>
#include <sys/types.h>
#include <boost/optional.hpp>
#include <sys/uio.h>

inline void throw_system_error_on(bool condition, const char * what_arg) {
    if(condition)
        throw std::system_error(errno, std::system_category(), what_arg);
}

class FileDesc{

public:
    FileDesc() = delete;

    FileDesc(const FileDesc&) = delete;

    FileDesc(FileDesc && f) : _fd(f._fd) { f._fd = -1; }

    ~FileDesc() { if(_fd != -1) ::close(_fd); }

    void operator = (const FileDesc&) = delete;

    FileDesc &operator = (FileDesc && f) {

        if(this != &f) {
            std::swap(_fd, f._fd);
            if(f._fd != -1) {
               f.close();
            }
        }

        return *this;
    }

    void close(){
        assert(_fd != -1);
        int fd = ::close(_fd);
        throw_system_error_on(fd == -1, "close");
        _fd = -1;
    }

    int get() const { return _fd; };

    static FileDesc open(std::string name, int flags, mode_t mode = 0) {
        int fd = ::open(name.c_str(), flags, mode);
        throw_system_error_on(fd == -1, "open");
        return FileDesc(fd);
    }

    static FileDesc socket(int family, int type, int protocol = 0) {
        int fd = ::socket(family, type, protocol);
        throw_system_error_on(fd == -1, "socket");
        return FileDesc(fd);
    }

//    static FileDesc epoll_create(int flags = 0) {
//        int fd = ::epoll_create1(flags);
//        throw_system_error_on(fd == -1, "epoll_create1");
//        return FileDesc(fd);
//    }

    FileDesc accept(sockaddr * sa, socklen_t& sl){
        int fd = ::accept(_fd, sa, &sl);
        throw_system_error_on(fd == -1, "accept");
        return FileDesc(fd);
    }

    void shutdown(int how) {
        int fd = ::shutdown(_fd, how);
        throw_system_error_on(fd == -1, "shutdown");
    }

    void bind(sockaddr * sa, socklen_t sl) {
        int fd = ::bind(_fd, sa, sl);
        throw_system_error_on(fd == -1, "bind");
    }

    void connect(sockaddr * sa, socklen_t sl) {
        int fd = ::connect(_fd, sa, sl);
        if (fd == -1 && errno == EINPROGRESS) {
            return;
        }

        throw_system_error_on(fd == -1, "connect");
    }

    void listen(int backlog) {
        int fd = ::listen(_fd, backlog);
        throw_system_error_on(fd == -1, "listen");
    }

    int setsockopt(int level, int optname, int data) {
        int fd = ::setsockopt(_fd, level, optname, &data, sizeof data + 1);
        throw_system_error_on(fd == -1, "setsockopt");
        return fd;
    }

    void set_non_blocking() {
        int flag;
        flag = fcntl(_fd, F_GETFL);
        flag |= O_NONBLOCK;
        ::fcntl(_fd, F_SETFL, flag);
    }

    boost::optional<size_t> read(void* buffer, size_t len) {
        int fd = ::read(_fd, buffer, len);
        if (fd == -1 && errno == EAGAIN) {
            return {};
        }
        throw_system_error_on(fd == -1, "read");
        return { size_t(fd) };
    }

    boost::optional<size_t> write(const void* buf, size_t len) {
        int fd = ::write(_fd, buf, len);
        if (fd == -1 && errno == EAGAIN) {
            return {};
        }
        throw_system_error_on(fd == -1, "write");
        return { size_t(fd) };
    }

    boost::optional<ssize_t> recv(void* buffer, size_t len, int flags) {
        int fd = ::recv(_fd, buffer, len, flags);
        if (fd == -1 && errno == EAGAIN) {
            return {};
        }
        throw_system_error_on(fd == -1, "recv");
        return { ssize_t(fd) };
    }

    boost::optional<size_t> send(const void* buffer, size_t len, int flags) {
        int fd = ::send(_fd, buffer, len, flags);
        if (fd == -1 && errno == EAGAIN) {
            return {};
        }
        throw_system_error_on(fd == -1, "send");
        return { size_t(fd) };
    }

    boost::optional<size_t> writev(const iovec *iov, int iovcnt) {
        int fd = ::writev(_fd, iov, iovcnt);
        if (fd == -1 && errno == EAGAIN) {
            return {};
        }
        throw_system_error_on(fd == -1, "writev");
        return { size_t(fd) };
    }

    boost::optional<size_t> readv(const iovec *iov, int iovcnt) {
        int fd = ::readv(_fd, iov, iovcnt);
        if (fd == -1 && errno == EAGAIN) {
            return {};
        }
        throw_system_error_on(fd == -1, "readv");
        return { size_t(fd) };
    }

private:
    int _fd;
    FileDesc(int fd) : _fd(fd) {}

};

#endif //ZEUS_FILEDESC_H
