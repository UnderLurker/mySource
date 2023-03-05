#pragma once
#define NAME_SPACE_START(name) namespace name {
#define NAME_SPACE_END() }

#define OFFSET(className,fieldName) (size_t)&(((className*)0)->fieldName)

#include <string>
#include <map>
#ifndef _UTIL_
#define _UTIL_



NAME_SPACE_START(myUtil)

#define REMOVE_SPACE(str) \
    for(auto it=str.begin();it!=str.end();){    \
        if(*it==' ') it=str.erase(it);      \
        else break;                         \
    }

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


NAME_SPACE_END()

#endif //!_UTIL_