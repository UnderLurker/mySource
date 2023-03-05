#pragma once

#include "Util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#ifndef _LOGGER_
#define _LOGGER_

NAME_SPACE_START(Log)

class Logger{
public:
    Logger() = delete;
    Logger(const Logger&) = delete;
    Logger(std::string logFilePath = "", std::string logFileName = "");
    ~Logger() = default;
    void LogStart(std::string context);
    void LogEnd(std::string context);
    void Debug(std::string context);
    void Error(std::string context);
    void Warning(std::string context);
    void Info(std::string context);
    bool OpenFile(std::string absolutePath);
    bool CloseFile();
    std::stringstream GetCurrentTime();
public:
    static std::string m_logFilePath;
    static std::string m_title;
private:
    std::ofstream _file;
    std::string absPath;
};

NAME_SPACE_END()
#endif //!_LOGGER_