#pragma once

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <list>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#include "SingleLinkList.h"
#include "Util.h"

#ifndef _LOGGER_
#define _LOGGER_

NAME_SPACE_START(myUtil)

#undef ERROR
enum LogType {
    INFO = 0,
    DEBUG,
    WARNING,
    ERROR
};

//class CLog {
//public:
//    CLog() = delete;
//    CLog(const string& logPath, const string& logName = "CLog.txt")
//        : filePath(logPath), fileName(logName) {}
//    CLog(const CLog&)  = delete;
//    CLog(const CLog&&) = delete;
//    virtual ~CLog() {
//        end();
//    }
//    CLog operator=(const CLog&) = delete;
//
//    void end() {
//        _endFlag = true;
//        _listCondition.notify_all();
//    }
//
//    void push(const std::string& info, LogType type = INFO) {
//        std::unique_lock<std::mutex> lock(_push);
//        _pushCondition.wait(lock, []() { return true; });
//        if (type >= logType) _logCache.emplace_back(info);
//        lock.unlock();
//        _pushCondition.notify_all();
//        _listCondition.notify_all();
//    }
//
//    void writeLog(const string& info) {
//        std::unique_lock<std::mutex> lock(_write);
//        _writeCondition.wait(lock, [] { return true; });
//        if (_of.bad()) {
//            _writeCondition.notify_all();
//            return;
//        }
//        _of << info;
//        _of.flush();
//        lineNumber++;
//        lock.unlock();
//        _writeCondition.notify_all();
//    }
//
//    void work() {
//        _of.open(filePath + "/" + fileName, ios::app | ios::out);
//        if (_of.fail()) return;
//        while (!_endFlag) {
//            std::unique_lock<std::mutex> lock(_listEmpty);
//            _listCondition.wait(lock, [this] { return _endFlag || !_clogList.empty() || !_logCache.empty(); });
//            std::unique_lock<std::mutex> pushLock(_push);
//            _pushCondition.wait(pushLock, []() { return true; });
//            if (!_logCache.empty()) {
//                for (const auto& item : _logCache) {
//                    _clogList.push(item);
//                }
//                _logCache.clear();
//            }
//            if (!_clogList.empty()) {
//                writeLog(_clogList.front().value());
//                _clogList.pop();
//            }
//            pushLock.unlock();
//            lock.unlock();
//            _pushCondition.notify_all();
//        }
//        cout << "work end" << endl;
//        _of.close();
//    }
//
//public:
//    std::string filePath {"."};
//    std::string fileName {"CLog.txt"};
//    uint32_t lineNumber {0};
//    LogType logType {INFO};
//
//private:
//    std::ofstream _of;
//    std::list<std::string> _logCache;
//    SingleLinkList<std::string> _clogList;
//    std::condition_variable _listCondition;
//    std::condition_variable _pushCondition;
//    std::condition_variable _writeCondition;
//    std::mutex _push;
//    std::mutex _listEmpty;
//    std::mutex _write;
//    bool _endFlag {false};
//};
//
//#define CLOG_DECLARE_1(method, logPath)     \
//    myUtil::CLog utilClog##method(logPath); \
//    std::thread logThread##method(&utilClog##method.work, utilClog##method);
//
//#define CLOG_DECLARE_2(method, logPath, logName)     \
//    myUtil::CLog utilClog##method(logPath, logName); \
//    std::thread logThread##method(&utilClog##method.work, utilClog##method);

class CLog {
public:
    static void end() {
        _endFlag = true;
        _listCondition.notify_all();
    }

    static void push(const std::string& info, LogType type = INFO) {
        if (type < logType) return;
        std::unique_lock<std::mutex> lock(_push);
        _pushCondition.wait(lock, []() { return true; });
        _logCache.emplace_back(info);
        lock.unlock();
        _pushCondition.notify_all();
        _listCondition.notify_all();
    }

    static void writeLog(const string& info) {
        std::unique_lock<std::mutex> lock(_write);
        _writeCondition.wait(lock, [] { return true; });
        if (_of.bad()) {
            _writeCondition.notify_all();
            return;
        }
        _of << info;
        _of.flush();
        lineNumber++;
        lock.unlock();
        _writeCondition.notify_all();
    }

    static void work() {
        _of.open(filePath + "/" + _fileName, ios::app | ios::out);
        if (_of.fail()) return;
        while (!_endFlag) {
            std::unique_lock<std::mutex> lock(_listEmpty);
            _listCondition.wait(lock, [] { return _endFlag || !_clogList.empty() || !_logCache.empty(); });
            std::unique_lock<std::mutex> pushLock(_push);
            _pushCondition.wait(pushLock, []() { return true; });
            if (!_logCache.empty()) {
                for (const auto& item : _logCache) {
                    _clogList.push(item);
                }
                _logCache.clear();
            }
            if (!_clogList.empty()) {
                writeLog(_clogList.front().value());
                _clogList.pop();
            }
            pushLock.unlock();
            lock.unlock();
            _pushCondition.notify_all();
        }
        cout << "work end" << endl;
        _of.close();
    }
    static std::string filePath;
    static uint32_t lineNumber;
    static LogType logType;

private:
    static std::ofstream _of;
    static std::list<std::string> _logCache;
    static SingleLinkList<std::string> _clogList;
    static std::condition_variable _listCondition;
    static std::condition_variable _pushCondition;
    static std::condition_variable _writeCondition;
    static std::mutex _push;
    static std::mutex _listEmpty;
    static std::mutex _write;
    static bool _endFlag;
    static std::string _fileName;
};
std::string CLog::filePath = ".";
uint32_t CLog::lineNumber  = 0;
LogType CLog::logType = INFO;
std::ofstream CLog::_of;
std::list<std::string> CLog::_logCache;
SingleLinkList<std::string> CLog::_clogList;
std::condition_variable CLog::_listCondition;
std::condition_variable CLog::_pushCondition;
std::condition_variable CLog::_writeCondition;
std::mutex CLog::_push;
std::mutex CLog::_listEmpty;
std::mutex CLog::_write;
bool CLog::_endFlag         = false;
std::string CLog::_fileName = "CLog.txt";
thread Work(CLog::work);

NAME_SPACE_END()
#endif //!_LOGGER_
