//
// Created by comoon on 4/26/17.
//

#ifndef ZEUS_LOG_H
#define ZEUS_LOG_H

#include "Singleton.h"

#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <cstring>

using namespace std;

class Log : public Singleton<Log> {

public:
    Log() { createLogFile(); }
    ~Log() { if(_ost.is_open()) _ost.close(); }

    template <typename T>
    Log & operator << (T& t) {
        _ost << t;
        return *this;
    }

    ostream & putLog(const char * lv) {
        time_t nowTime = createLogFile();
        if(0 == strcmp(lv, "DEBUG")) {
            std::cout << put_time(std::localtime(&nowTime), "%F %T") << " [" << lv << "] ";
            return std::cout;
        }

        _ost << put_time(std::localtime(&nowTime), "%F %T");
        _ost << " [" << lv << "] ";
        return _ost;
    }

private:
    time_t createLogFile() {
        chrono::system_clock::time_point now = chrono::system_clock::now();
        time_t nowTime = chrono::system_clock::to_time_t(now);
        uint64_t tmpDay = nowTime / 86400;

        if(tmpDay == _currentDay)
            return nowTime;

        if(_ost.is_open())
            _ost.close();

        string timeStr;
        stringstream ss;
        ss << put_time(std::localtime(&nowTime), "%F %T");
        ss >> timeStr;

        _ost.open("./" + timeStr + " " + _logName, std::ios::app | std::ios::out);
        return nowTime;
    }

private:
    ofstream _ost;
    uint64_t _currentDay{0};
    string _logName {"Zeus.log"};
};

#define LOG_OBJ Log::Instance()
#define LOG_INFO LOG_OBJ.putLog("INFO")
#define LOG_WARN LOG_OBJ.putLog("WARN")
#define LOG_ERROR LOG_OBJ.putLog("ERROR")
#define LOG_DEBUG LOG_OBJ.putLog("DEBUG")
#define LOG_TRACE LOG_OBJ.putLog("TRACE")


#endif //ZEUS_LOG_H
