#pragma once

#include "Util.h"
#include "SingleLinkList.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <mutex>
#include <list>
#include <condition_variable>
#include <thread>

#ifndef _LOGGER_
#define _LOGGER_

NAME_SPACE_START(myUtil)

#define BUFSIZE 4096//格式化后的字符串最大缓存
#undef ERROR
enum LogType{
    INFO=0,
    DEBUG,
    WARNING,
    ERROR
};

class Logger{
public:
    Logger() = default;
    Logger(const Logger&) = delete;
    ~Logger() = default;
    static void Init(const std::string& iniPath);
    static void LogStart(const wchar_t* fmt, ...);
    static void LogEnd(const char* fmt, ...);
    static void Debug(const char* fmt, ...);
    static void Error(const char* fmt, ...);
    static void Warning(const char* fmt, ...);
    static void Info(const char* fmt, ...);
    static std::stringstream GetCurrentTime();
public:
    static std::string filePath;
    static LogType LogLevel;
};

class CLog {
    static thread th;
public:
    static void join()
    {
        if (th.joinable()) th.join();
    }

    static void end()
    {
        _endFlag = true;
        _listCondition.notify_all();

    }

    static void push(const std::string& info)
    {
        std::unique_lock<std::mutex> lock(_push);
        _pushCondition.wait(lock, []() { return true; });
        _logCache.emplace_back(info);
        lock.unlock();
        _pushCondition.notify_all();
        _listCondition.notify_all();
    }

    static void writeLog(const string& info)
    {
        ofstream of(_fileName, ios::app | ios::out);
        if (of.fail())
            return;
        of << info << endl;
    }

    static void work()
    {
        while (!_endFlag) {
            std::unique_lock<std::mutex> lock(_listEmpty);
            _listCondition.wait(lock, [] {
                if (_clogList.empty() && _logCache.empty()){
#ifdef _DEBUG_
                    cout<<"false"<<endl;
#endif
                    return false;
                }
#ifdef _DEBUG_
                if (!_clogList.empty())
                    for (const auto& item : _clogList){
                        cout<<item.value()<<" ";
                    }
                else
                    for (const auto& item : _logCache){
                        cout<<item<<" ";
                    }
#endif
                return true;
            });
            _pushCondition.wait(lock, []() { return true; });
            for (const auto& item : _logCache) {
                _clogList.push(item);
            }
            _logCache.clear();
            writeLog(_clogList.front().value());
            lock.unlock();
            _clogList.pop();
            _pushCondition.notify_all();
        }
    }

private:
    static std::list<std::string> _logCache;
    static SingleLinkList<std::string> _clogList;
    static std::condition_variable _listCondition;
    static std::condition_variable _pushCondition;
    static std::mutex _push;
    static std::mutex _listEmpty;
    static bool _endFlag;
    static std::string _fileName;
};

std::list<std::string> CLog::_logCache;
SingleLinkList<std::string> CLog::_clogList;
std::condition_variable CLog::_listCondition;
std::condition_variable CLog::_pushCondition;
std::mutex CLog::_push;
std::mutex CLog::_listEmpty;
bool CLog::_endFlag = false;
std::string CLog::_fileName = "CLog.txt";
thread CLog::th = thread(CLog::work);

NAME_SPACE_END()
#endif //!_LOGGER_