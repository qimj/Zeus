//
// Created by comoon on 5/20/17.
//

#ifndef ZEUS_IOBUF_H
#define ZEUS_IOBUF_H

#include <iostream>
#include <memory>
#include <cstring>
#include <cassert>

class IOBuf {
public:

    IOBuf(size_t len) : _currentEnd(0), _len(len) {
        _buf = std::shared_ptr<char>(new char[len], [](char *p) { delete[] p; });
    }
    ~IOBuf(){}

    IOBuf(const IOBuf& buf) = delete;
    IOBuf& operator = (const IOBuf& buf) = delete;
    IOBuf(IOBuf&& buf) : _buf(buf._buf), _currentEnd(buf._currentEnd), _len(buf._len){
    }

    IOBuf& operator = (IOBuf&& buf){
        if(this != &buf){
            _buf = buf._buf;
            _currentEnd = buf._currentEnd;
            _len = buf._len;
        }
        return *this;
    }

    bool Put(const char * src, size_t len) {
        assert(_currentEnd + len <= _len);
        memcpy(_buf.get() + _currentEnd, src, len);
        _currentEnd += len;
    }

    std::shared_ptr<char> Get() { return _buf; }
    size_t Len() { return _len; }
    void Print() { std::cout << _buf.get() << std::endl; }
    void Print(size_t index) { std::cout << _buf.get() + index << std::endl; }
    void Forward(size_t len) { _currentEnd+= len; }

private:
    std::shared_ptr<char> _buf;
    size_t _currentEnd;
    size_t _len;
};

#endif //ZEUS_IOBUF_H
