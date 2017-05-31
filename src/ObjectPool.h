//
// Created by comoon on 5/25/17.
//

#ifndef ZEUS_OBJECTPOOL_H
#define ZEUS_OBJECTPOOL_H

#include <stack>
#include <iostream>

#include "Singleton.h"

template  <typename T>
class ObjectPool : public Singleton<ObjectPool<T>> {

private:
    std::stack<T*> _free;
    unsigned _freeCnt;
    unsigned _totalCnt;

public:
    ObjectPool() : _freeCnt(0), _totalCnt(0) {}

    //forbide to copy
    ObjectPool(const ObjectPool &) = delete;

    //move constructor, so we can move it
    ObjectPool(ObjectPool && other) {
        this->_free(std::move(other._free));
    }

    //free all of the memory chunks
    ~ObjectPool() {
        while(!_free.empty()) {
            ::operator delete(_free.top());
            _free.pop();
        }
    }

    //allocate a chunk of memory as big as Type T needs
    void * Allocate() {
        void * ptr = nullptr;
        if(_free.empty()) {
            ptr = ::operator new(sizeof(T));
            _totalCnt ++;
        }
        else {
            ptr = static_cast<void *>(_free.top());
            _free.pop();
            _freeCnt --;
        }
        return ptr;
    }

    //add memory to the pool
    void Deallocate(T * p) {
        p->~T();
        _free.push(p);
        _freeCnt ++;
    }

    unsigned GetFreeCnt() { return _freeCnt; }
    unsigned GetTotalCnt() { return _totalCnt; }
};

template <typename T>
class PoolObject {
public:
    inline void * operator new(size_t size) {
        //std::cout << "new total: " << ObjectPool<T>::Instance().GetTotalCnt() << " free: " << ObjectPool<T>::Instance().GetFreeCnt() << std::endl;

        if(size != sizeof(T))
            return ::operator new(size);
        else
            return ObjectPool<T>::Instance().Allocate();
    }

    inline void operator delete(void * ptr, size_t size) {
        if(size != sizeof(T))
            ::operator delete(ptr);
        else
            ObjectPool<T>::Instance().Deallocate((T*)ptr);

        //std::cout << "delete total: " << ObjectPool<T>::Instance().GetTotalCnt() << " free: " << ObjectPool<T>::Instance().GetFreeCnt() << std::endl;
    }
};

#endif //ZEUS_OBJECTPOOL_H
