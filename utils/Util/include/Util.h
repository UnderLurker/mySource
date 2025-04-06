#pragma once
#include <bitset>
#include <condition_variable>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <functional>
#include <ios>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <vector>
#define NAME_SPACE_START(name) namespace name {
#define NAME_SPACE_END() }

#define OFFSET(className,fieldName) (size_t)&(((className*)0)->fieldName)

using namespace std;
#ifndef _UTIL_
#define _UTIL_

NAME_SPACE_START(myUtil)

#define REMOVE_SPACE(str)                                                      \
  for (auto it = str.begin(); it != str.end();) {                              \
    if (*it == ' ')                                                            \
      it = str.erase(it);                                                      \
    else                                                                       \
      break;                                                                   \
  }

#define FREE_VECTOR_LP(vectorName, _row)                                       \
  for (auto item : vectorName) {                                               \
    for (int i = 0; i < _row; i++)                                             \
      delete[] item[i];                                                        \
    delete[] item;                                                             \
  }                                                                            \
  vectorName.clear();

//释放二维指针
#define FREE_LP_2(lpName, row)                                                 \
  for (int i = 0; i < row; i++) {                                              \
    delete[] lpName[i];                                                        \
  }                                                                            \
  delete[] lpName;

//释放三维指针
#define FREE_LP_3(lpName, men1, men2)                                          \
  for (int x = 0; x < men1; x++) {                                             \
    for (int y = 0; y < men2; y++) {                                           \
      delete[] lpName[x][y];                                                   \
    }                                                                          \
    delete[] lpName[x];                                                        \
  }                                                                            \
  delete[] lpName;

#define CHECK_NULL_VOID(ptr) \
    do {                     \
        if (ptr == nullptr)  \
            return;          \
    } while (false);

#define CHECK_NULL_RETURN(ptr, value) \
    do {                              \
        if (ptr == nullptr)           \
            return value;             \
    } while (false);

#ifdef UTIL_DEBUG
#include <cassert>
#define MYASSERT(info) assert(info);
#else
#define MYASSERT(info) static_cast<void*>(0);
#endif

static std::string CurrTime() {
    char tmp[64];
    time_t ptime;
    time(&ptime);
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S ", localtime(&ptime));
    return {tmp};
}

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
#ifdef LOG_FILE_PATH
    std::cout << file.substr(file.rfind('/') + 1) << ":" << line << " ";
#endif
    std::cout << function << " ";
    std::cout << strLevel;
    std::cout << buffer;
    std::cout << std::endl;
}

#ifdef UTIL_DEBUG
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

std::string getFile(const std::string& filePath);

class Timer{
private:
    clock_t startTime;
    clock_t curTime;
    void* lpFunc{nullptr};
    //false运行一次 true一直运行
    bool runFlag{false};
    long inter{100};
public:
    Timer();
    Timer(void* lpFunc);
    //获取运行时间
    std::string getRunTime(const std::string& format = "millisecond");
    //设置隔interval就运行lpFunc
    void setInterval(void* lpFunc,long interval = 100);
    //开始运行 使用的是function
    template<typename classType, typename ...Args>
    void start(Args... args);
    //结束运行
    void end();
};

vector<string> Split(const string& source, char ch);
vector<string> Split(const char* source,size_t len,char ch);
vector<wstring> Split(const wstring& source, wchar_t ch);
vector<wstring> Split(const wchar_t* source,size_t len,wchar_t ch);

class ProgressBar{
private:
	class Logic{
	public:
		static int barLen;
		static int curLen;
		static string str;
		static condition_variable _cv;
		static char ch;
		void operator()(){
			unique_lock<mutex> lock(barMutex);
			for(int i=0;i<=barLen;i++){
				_cv.wait(lock,[]()->bool{
					return !ProgressBar::_pause;
				});
				str[i]=ch;
				cout<<"\r|"<<str<<"| "<<(int)i*100/barLen<<"%";
			}
		}
	};
public:
	static void Start(const int _barLen = 100, const char _ch = '='){
		ProgressBar::Logic::barLen=_barLen;
		ProgressBar::Logic::ch=_ch;
		ProgressBar::_pause=false;
		ProgressBar::Logic::str=string(_barLen,' ');
		ProgressBar::run = thread(Logic());
	}
	// static void Start(){run.join();}
    static void Pause(){
		ProgressBar::_pause=true;
	}
    static void Continue(){
		ProgressBar::_pause=false;
		Logic::_cv.notify_one();
	}
public:
	static bool _pause;
    static mutex barMutex;
    static thread run;
};


NAME_SPACE_END()

#endif //!_UTIL_
