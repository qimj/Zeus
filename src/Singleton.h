//
// Created by comoon on 4/26/17.
//

#ifndef ZEUS_SINGLETON_H
#define ZEUS_SINGLETON_H

#include <mutex>
#include <cstdlib>

template <typename T>
class Singleton {

private:
    static T* volatile _instance;

    static void Destroy() {
        if(_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
        }
    }

public:
    static T & Instance() {
        if(_instance == nullptr) {
            _instance = new T;
            atexit(Destroy);
        }
        return *_instance;
    }

};

template <typename T>
T * volatile Singleton<T >::_instance = nullptr;

template <typename T>
class MultiThreadSingleton {

private:
    static T* volatile _instance;
    static std::mutex mu;

    static void Destroy() {
        if(_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
        }
    }

public:
    static T & Instance() {
        if(_instance == nullptr) {
            std::lock_guard<std::mutex> lock(mu);
            _instance = new T;
            atexit(Destroy);
        }
        return *_instance;
    }

};

template <typename T>
T * volatile MultiThreadSingleton<T>::_instance = nullptr;

template <typename T>
std::mutex MultiThreadSingleton<T>::mu;


#endif //ZEUS_SINGLETON_H
