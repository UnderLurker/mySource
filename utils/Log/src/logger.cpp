#include "logger.h"
#include <ctime>
#include <exception>
#include <fstream>
#include <ios>
#include <iterator>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <time.h>

std::string basePath = "C:\\";  //日志路径
std::string baseTitle = "logger.txt"; //日志文件名

NAME_SPACE_START(Log)

std::string Logger::m_logFilePath = basePath;
std::string Logger::m_title = baseTitle;

Logger::Logger(std::string logFilePath, std::string logFileName){
    std::string absolutePath = "";
    if(logFilePath != ""){
        absolutePath += logFilePath;
    }
    else{
        absolutePath += Logger::m_logFilePath;
    }
    if(logFileName != ""){
        absolutePath += logFileName;
    }
    else{
        absolutePath += Logger::m_title;
    }
    this->absPath = absolutePath;
}

void Logger::LogStart(std::string context = ""){
    if(!this->OpenFile(this->absPath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"------------------------------Log Start"
         <<" "<<context<<" "
         <<"------------------------------"<<std::endl;
    this->CloseFile();
}
void Logger::LogEnd(std::string context = ""){
    if(!this->OpenFile(this->absPath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"------------------------------Log End"
         <<" "<<context<<" "
         <<"------------------------------"<<std::endl;
    this->CloseFile();
}
void Logger::Debug(std::string context = ""){
    if(!this->OpenFile(this->absPath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"[Log Debug]:"
         <<context<<std::endl;
    this->CloseFile();
}
void Logger::Error(std::string context = ""){
    if(!this->OpenFile(this->absPath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"[Log Error]:"
         <<context<<std::endl;
    this->CloseFile();
}
void Logger::Warning(std::string context = ""){
    if(!this->OpenFile(this->absPath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"[Log Warning]:"
         <<context<<std::endl;
    this->CloseFile();
}
void Logger::Info(std::string context = ""){
    if(!this->OpenFile(this->absPath)) return;
    std::stringstream ss=GetCurrentTime();
    _file<<ss.str()
         <<"[Log Info]:"
         <<context<<std::endl;
    this->CloseFile();
}
bool Logger::OpenFile(std::string absolutePath){
    try {
        this->_file.open(absolutePath, std::ios::out | std::ios::app);
        if(!_file.is_open()){
            return false;
        }
        return true;
    } catch (std::exception ex) {
        return false;
    }
}

bool Logger::CloseFile(){
    try{
        this->_file.close();
        return true;
    }
    catch(std::exception ex){
        return false;
    }
}

std::stringstream Logger::GetCurrentTime(){
    std::stringstream ss;
    time_t now=time(nullptr);
    tm curr_tm;
    localtime_s(&curr_tm, &now);
    ss<<curr_tm.tm_year<<"-"<<curr_tm.tm_mon<<"-"<<curr_tm.tm_yday
      <<" "<<curr_tm.tm_hour<<":"<<curr_tm.tm_min<<":"<<curr_tm.tm_sec
      <<" ";
    return ss;
}

NAME_SPACE_END()