#include <stdio.h>
#include <vadefs.h>
#define _CRT_SECURE_NO_WARNINGS
#include "logger.h"
#include <chrono>
#include <ctime>
#include <tchar.h>
#include <exception>
#include <iomanip>
#include <sstream>
#include <stdarg.h>

NAME_SPACE_START(myUtil)

std::ofstream _file;

std::string Logger::filePath = "logger.txt";
LogType Logger::LogLevel = INFO;

bool OpenFile(std::string filePath){
    try {
        _file.open(filePath, std::ios::out | std::ios::app);
        if(!_file.is_open()){
            return false;
        }
        return true;
    } catch (std::exception ex) {
        return false;
    }
}

bool CloseFile(){
    try{
        _file.close();
        return true;
    }
    catch(std::exception ex){
        return false;
    }
}

void Logger::Init(const std::string& iniPath){
    IniHelper ini(iniPath);
    Logger::filePath = (string)ini.getIniConfig("logger", "loggerFilePath", "./logger.txt");
    std::string t = (string)ini.getIniConfig("logger", "loggerLevel", "INFO");
    LogType iniLevel=INFO;
    if(t=="DEBUG") iniLevel=DEBUG;
    else if(t=="WARNING") iniLevel=WARNING;
    else if(t=="ERROR") iniLevel=ERROR;
    Logger::LogLevel=iniLevel;
}

void Logger::LogStart(const wchar_t* fmt, ...){
    if(!OpenFile(Logger::filePath)) return;
    wchar_t *buf=new wchar_t[BUFSIZE];
    va_list args;
    va_start(args,fmt);
    _vswprintf(buf, fmt, args);
    va_end(args);
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"------------------------------Log Start"
         <<" "<<*buf<<" "
         <<"------------------------------"<<std::endl;
    CloseFile();
}
void Logger::LogEnd(const char* context, ...){
    if(!OpenFile(Logger::filePath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"------------------------------Log End"
         <<" "<<context<<" "
         <<"------------------------------"<<std::endl;
    CloseFile();
}
void Logger::Debug(const char* context, ...){
    if(DEBUG<Logger::LogLevel) return;
    if(!OpenFile(Logger::filePath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"[Debug]:"
         <<context<<std::endl;
    CloseFile();
}
void Logger::Error(const char* context, ...){
    if(ERROR<Logger::LogLevel) return;
    if(!OpenFile(Logger::filePath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"[Error]:"
         <<context<<std::endl;
    CloseFile();
}
void Logger::Warning(const char* context, ...){
    if(WARNING<Logger::LogLevel) return;
    if(!OpenFile(Logger::filePath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"[Warning]:"
         <<context<<std::endl;
    CloseFile();
}
void Logger::Info(const char* context, ...){
    if(INFO<Logger::LogLevel) return;
    if(!OpenFile(Logger::filePath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"[Info]:"
         <<context<<std::endl;
    CloseFile();
}

std::stringstream Logger::GetCurrentTime(){
    time_t time=chrono::system_clock::to_time_t(chrono::system_clock::now());
    std::stringstream ss;
    ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss;
}

NAME_SPACE_END()