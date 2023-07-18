#pragma once

#include "Util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

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

NAME_SPACE_END()
#endif //!_LOGGER_