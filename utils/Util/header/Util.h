#pragma once
#include <cstddef>
#include <time.h>
#include <vadefs.h>
#define NAME_SPACE_START(name) namespace name {
#define NAME_SPACE_END() }

#define OFFSET(className,fieldName) (size_t)&(((className*)0)->fieldName)

#include <string>
#include <vector>
#include <map>
using namespace std;
#ifndef _UTIL_
#define _UTIL_



NAME_SPACE_START(myUtil)

#define REMOVE_SPACE(str) \
    for(auto it=str.begin();it!=str.end();){    \
        if(*it==' ') it=str.erase(it);      \
        else break;                         \
    }

#define FREE_VECTOR_LP(vectorName,_row) \
    for(auto item : vectorName){	\
		for(int i=0;i<_row;i++)\
			delete [] item[i];\
        delete [] item;	\
    }\
	vectorName.clear();

//释放二维指针
#define FREE_LP_2(lpName,row) \
	for(int i=0;i<row;i++){\
		delete [] lpName[i];\
	}\
	delete [] lpName;
    
#define INI_FILE_PATH "config.ini"

class IniValue{
public:
    friend class IniHelper;
    IniValue();
    IniValue(const bool val);
    IniValue(const int val);
    IniValue(const char* val);
    IniValue(const std::string val);
    IniValue& operator=(const bool& val);
    IniValue& operator=(const int& val);
    IniValue& operator=(const char* val);
    IniValue& operator=(const std::string& val);
    operator bool() const;
    operator int() const;
    operator std::string() const;
private:
    std::string m_value;
};

class IniHelper{
private:
    std::map<std::string, std::map<std::string, IniValue>> m_map;
    std::string m_filePath{INI_FILE_PATH};
public:
    IniHelper();
    IniHelper(const std::string& filePath);
    void saveIniConfig();
    void LogIniConfig() const;
    void setIniConfig(const std::string& sectionName,const std::string& keyName,const std::string& value);
    IniValue getIniConfig(const std::string& sectionName,const std::string& keyName,const std::string& defValue) const;
    bool removeIniConfig(const std::string& sectionName,const std::string& keyName);
protected:
    void loadIniFile();
    void trim(std::string& str);
};


template<typename T>
class Function;

template<typename T,typename... Args>
class Function<T(Args...)>{
    using Method = T(*)(Args...);
public:
    Function(Method method):func(method){}
    inline T operator()(Args... args)
    {
        return func(args...);
    }
private:
    Method func;
};

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

NAME_SPACE_END()

#endif //!_UTIL_
