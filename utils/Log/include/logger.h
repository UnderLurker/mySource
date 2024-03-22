#pragma once

#ifndef _LOGGER_H
#define _LOGGER_H

#include <condition_variable>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <thread>

#include "SingleLinkList.h"
#include "Util.h"

#define CLOG_MAX_LEN 400

NAME_SPACE_START(myUtil)

enum LogType {
    INFO = 0,
    DEBUG,
    WARNING,
    ERROR
};

unique_ptr<char[]> getTimeString() {
    time_t nowTime;
    time(&nowTime);
    tm p {};
    localtime_s(&p, &nowTime);
    auto str = make_unique<char[]>(9);
    snprintf(str.get(), 9, "%02d:%02d:%02d", p.tm_hour, p.tm_min, p.tm_sec);
    return str;
}

unique_ptr<char[]> getDayString() {
    time_t nowTime;
    time(&nowTime);
    tm p {};
    localtime_s(&p, &nowTime);
    auto str = make_unique<char[]>(15);
    snprintf(str.get(), 15, "%04d-%02d-%02d.txt", p.tm_year + 1900, p.tm_mon + 1, p.tm_mday);
    return str;
}

class CLog {
public:
    CLog() = delete;
    explicit CLog(const char* logPath, const char* logName = "CLog.txt")
        : filePath(logPath), fileName(logName) {}
    CLog(const CLog&)  = delete;
    CLog(const CLog&&) = delete;
    virtual ~CLog() { end(); }
    CLog operator=(const CLog&) = delete;

    void end() {
        _endFlag = true;
        _listCondition.notify_all();
    }

    void push(const std::string& info, LogType type = INFO) {
        std::unique_lock<std::mutex> lock(_push);
        _pushCondition.wait(lock, []() { return true; });
        if (type >= logType) _logCache.emplace_back(info);
        lock.unlock();
        _pushCondition.notify_all();
        _listCondition.notify_all();
    }

    void writeLog(const string& info) {
        std::unique_lock<std::mutex> lock(_write);
        _writeCondition.wait(lock, [] { return true; });
        if (_of.bad()) {
            _writeCondition.notify_all();
            return;
        }
        _of << info;
        if (lineNumber % 10 == 0) _of.flush();
        lineNumber++;
        lock.unlock();
        _writeCondition.notify_all();
    }

    void work() {
        if (filePath.is_relative()) filePath = std::filesystem::absolute(filePath);
        if (!exists(filePath) && !create_directory(filePath)) {
            std::cout << "CLog directory create fail!!!\n";
            return;
        }
        std::string fullPath = filePath.string() + "/" + fileName;

        _of.open(fullPath, ios::app | ios::out);
        if (_of.fail()) return;
        while (!_endFlag) {
            std::unique_lock<std::mutex> lock(_listEmpty);
            _listCondition.wait(lock, [this] { return _endFlag || !_clogList.empty() || !_logCache.empty(); });
            std::unique_lock<std::mutex> pushLock(_push);
            _pushCondition.wait(pushLock, []() { return true; });
            if (!_logCache.empty()) {
                for (const auto& item : _logCache) {
                    _clogList.push(item);
                }
                _logCache.clear();
            }
            pushLock.unlock();
            _pushCondition.notify_all();
            for (const auto& item : _clogList) {
                writeLog(item.value());
            }
            _clogList.clear();
            lock.unlock();
        }
        _of.close();
        std::cout << "CLog::work success end" << std::endl;
    }

public:
    std::filesystem::path filePath {"."};
    std::string fileName {"CLog.txt"};
    uint32_t lineNumber {0};
    LogType logType {INFO};

private:
    std::ofstream _of;
    std::list<std::string> _logCache;
    SingleLinkList<std::string> _clogList;
    std::condition_variable _listCondition;
    std::condition_variable _pushCondition;
    std::condition_variable _writeCondition;
    std::mutex _push;
    std::mutex _listEmpty;
    std::mutex _write;
    bool _endFlag {false};
};

#define CLOG_DECLARE_1(method, logPath)                           \
    myUtil::CLog utilClog##method(logPath, getDayString().get()); \
    std::thread logThread##method(&CLog::work, &utilClog##method);

#define CLOG_DECLARE_2(method, logPath, logName)     \
    myUtil::CLog utilClog##method(logPath, logName); \
    std::thread logThread##method(&CLog::work, &utilClog##method);

#define CLOG_END(method)    \
    utilClog##method.end(); \
    logThread##method.join();

#define CLOG_TYPE(method, type) utilClog##method.logType = type;

#define CLOG_PUSH(logType, method, fmt, ...)                   \
    do {                                                       \
        auto str = std::make_unique<char[]>(CLOG_MAX_LEN + 1); \
        snprintf(str.get(), CLOG_MAX_LEN, fmt, ##__VA_ARGS__); \
        utilClog##method.push(str.get(), logType);             \
    } while (false);

#ifdef WINDOWS
#define TrimFilePath(x) strrchr(x, '\\') ? strrchr(x, '\\') + 1 : x
#else //*nix
#define TrimFilePath(x) strrchr(x, '/') ? strrchr(x, '/') + 1 : x
#endif

#define LogDebug(method, fmt, ...)                                                                                  \
    CLOG_PUSH(myUtil::DEBUG, method, "[%s][%s:%d][DEBUG]:" fmt "\n", getTimeString().get(), TrimFilePath(__FILE__), \
              __LINE__, ##__VA_ARGS__)

#define LogInfo(method, fmt, ...)                                                                                 \
    CLOG_PUSH(myUtil::INFO, method, "[%s][%s:%d][INFO]:" fmt "\n", getTimeString().get(), TrimFilePath(__FILE__), \
              __LINE__, ##__VA_ARGS__)

#define LogWarn(method, fmt, ...)                                                               \
    CLOG_PUSH(myUtil::WARNING, method, "[%s][%s:%d][WARNING]:" fmt "\n", getTimeString().get(), \
              TrimFilePath(__FILE__), __LINE__, ##__VA_ARGS__)

#define LogError(method, fmt, ...)                                                                                  \
    CLOG_PUSH(myUtil::ERROR, method, "[%s][%s:%d][ERROR]:" fmt "\n", getTimeString().get(), TrimFilePath(__FILE__), \
              __LINE__, ##__VA_ARGS__)

NAME_SPACE_END()
#endif //!_LOGGER_H
