//
// Created by comoon on 5/20/17.
//

#ifndef ZEUS_IOBUF_H
#define ZEUS_IOBUF_H

#include "Util.h"

#include <iostream>
#include <memory>
#include <cstring>
#include <cassert>
#include <vector>

#define DYNAMIC_BUF_BLOCK_SIZE 65536

class IOBuf : Noncopyable {
public:
    IOBuf() {}
    IOBuf(size_t len) : _len(len) {}

    IOBuf(IOBuf&& buf) : _currentEnd(buf._currentEnd), _len(buf._len){
    }

    IOBuf& operator = (IOBuf&& buf){
        if(this != &buf){
            _currentEnd = buf._currentEnd;
            _len = buf._len;
        }
        return *this;
    }

    virtual bool Put(const char * src, size_t len) = 0;
    virtual char * getHead() const = 0;  //return it's head point and it may be invalid
    virtual char * getTail() const = 0;  //return it's current end and it's next write's start point, it may be invalid
    size_t Len() { return _len; }

protected:
    size_t _len;
    size_t _currentEnd{0};
};


class ConstIOBuf : public IOBuf{
public:

    ConstIOBuf(size_t len) : IOBuf(len) {
        _buf = std::shared_ptr<char>(new char[len], [](char *p) { delete[] p; });
    }
    ~ConstIOBuf(){}

    //TODO move things
//    ConstIOBuf(ConstIOBuf&& buf) : _buf(buf._buf){
//    }

//    ConstIOBuf& operator = (ConstIOBuf&& buf){
//        if(this != &buf){
//            _buf = buf._buf;
//            _currentEnd = buf._currentEnd;
//            _len = buf._len;
//        }
//        return *this;
//    }

    virtual bool Put(const char * src, size_t len) final {
        assert(_currentEnd + len <= _len);
        memcpy(_buf.get() + _currentEnd, src, len);
        _currentEnd += len;
    }

    virtual char * getTail() const final { return _buf.get() + _currentEnd; }
    virtual char * getHead() const final { return _buf.get(); }
    std::shared_ptr<char> Get() { return _buf; }

    void Print(size_t index = 0) { std::cout << _buf.get() + index << std::endl; }
    void Forward(size_t len) { _currentEnd+= len; }

private:
    std::shared_ptr<char> _buf;
};


class DynamicIOBuf : public IOBuf {
public:
    DynamicIOBuf() : IOBuf(DYNAMIC_BUF_BLOCK_SIZE), _buf(DYNAMIC_BUF_BLOCK_SIZE) {}
    ~DynamicIOBuf() {}

    virtual bool Put(const char * src, size_t len) final {
        TryToFit(len);
        memcpy((&_buf[_currentEnd]), src, len);
        _currentEnd += len;
    }

    virtual char * getHead() const final { return const_cast<char*>(&_buf[0]); }
    virtual char * getTail() const final { return const_cast<char*>(&_buf[_currentEnd]); }

    void Print(size_t index = 0) {
        assert(index <= _len);
        std::cout << &_buf[index] << std::endl;
    }

    void TryToFit(size_t len) {
        while(len > _len - _currentEnd) {
            _buf.resize(_len + DYNAMIC_BUF_BLOCK_SIZE);
            _len += DYNAMIC_BUF_BLOCK_SIZE;
        }
    }

    void Forward(size_t len) { _currentEnd+= len;}

private:
    std::vector<char> _buf;
};

#endif //ZEUS_IOBUF_H
