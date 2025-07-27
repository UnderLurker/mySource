
#ifndef _LOGGER_WRAPPER_H
#define _LOGGER_WRAPPER_H

#include <iostream>
#include <sstream>

#ifdef UTIL_DEBUG
#include "client/include/log_client.h"
#endif

namespace myUtil {

#ifdef UTIL_DEBUG

enum LogLevel {
    ERROR,
    DEBUG,
    WARNING,
    INFO
};

static std::string ToStrLevel(LogLevel level) {
    std::string strLevel;
    switch (level) {
        case ERROR: {
            strLevel = "[ERROR]";
            break;
        }
        case DEBUG: {
            strLevel = "[DEBUG]";
            break;
        }
        case WARNING: {
            strLevel = "[WARNING]";
            break;
        }
        case INFO: {
            strLevel = "[INFO]";
            break;
        }
    }
    strLevel.append(" ");
    return strLevel;
}
#ifndef LOGGER_CLIENT
static std::string CurrTime() {
    char tmp[64];
    time_t ptime;
    time(&ptime);
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S ", localtime(&ptime));
    return {tmp};
}

static void UtilPrint(LogLevel level,
                      const std::string& file,
                      uint32_t line,
                      const std::string& function,
                      const char* fmt,
                      ...) {
    va_list args;
    va_start(args, fmt);
    constexpr size_t bufferLen = 9999;
    char buffer[bufferLen];
    vsprintf(buffer, fmt, args);
    va_end(args);
    std::string strLevel = ToStrLevel(level);
#ifdef LOG_TIME
    std::cout << CurrTime();
#endif
    std::cout << strLevel;
#ifdef LOG_FILE_PATH
    std::cout << file.substr(file.rfind('/') + 1) << ":" << line << " ";
#endif
    std::cout << function << " ";
    std::cout << buffer;
    std::cout << std::endl;
}
#else
static void UtilPrint(LogLevel level,
                      const std::string& file,
                      uint32_t line,
                      const std::string& function,
                      const char* fmt,
                      ...) {
    va_list args;
    va_start(args, fmt);
    constexpr size_t bufferLen = 9999;
    char buffer[bufferLen];
    vsprintf(buffer, fmt, args);
    va_end(args);
    std::string strLevel = ToStrLevel(level);
    std::stringstream ss;
    ss << strLevel;
#ifdef LOG_FILE_PATH
    ss << file.substr(file.rfind('/') + 1) << ":" << line << " ";
#endif
    ss << function << " ";
    ss << buffer;
    auto client = LogClient::GetInstance();
    client->Send(ss.str());
}
#endif

#define LOGE(fmt, ...) UtilPrint(LogLevel::ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) UtilPrint(LogLevel::DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) UtilPrint(LogLevel::WARNING, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) UtilPrint(LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#else
#define LOGE(fmt, ...)
#define LOGD(fmt, ...)
#define LOGI(fmt, ...)
#define LOGW(fmt, ...)
#endif
} // namespace myUtil
#endif //!_LOGGER_WRAPPER_H
