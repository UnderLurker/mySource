#pragma once
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <stdint.h>
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

//github上找的，我写的我也找不出问题在哪里
class MD5{
public:
  static string encodeFile(const string& filePath);
  static string encodeStr(const string& str);
};

const static char BaseTable[]={
  'A','B','C','D','E','F','G','H','I','J','K',
  'L','M','N','O','P','Q','R','S','T','U',
  'V','W','X','Y','Z','a','b','c','d','e',
  'f','g','h','i','j','k','l','m','n','o',
  'p','q','r','s','t','u','v','w','x','y',
  'z','0','1','2','3','4','5','6','7','8',
  '9','+','/'
};

class Base64{
public:
  static string encoding(string str){
    string res="";
    for(int i=0;i<str.size();i+=3){
      string t="";
      for(int j=0;j<3&&i+j<str.size();j++){
        t.append(bitset<8>(str[i+j]).to_string());
      }
      int pos=0,count=0;
      while(t.size()-pos>=6){
        int index=0;
        for(int j=pos;j<pos+6;j++){
          index=index*2+t[j]-'0';
        }
        res.append(string(1,BaseTable[index]));
        pos+=6;
        count++;
      }
      int zeroCount=6-t.size()+pos,index=0;
      if(zeroCount<6){
        for(int j=pos;j<t.size();j++){
          index=index*2+t[j]-'0';
        }
        index<<=zeroCount;
        res.append(string(1,BaseTable[index]));
        count++;
      }
      if(count!=4){
        res.append(string(4-count,'='));
      }
    }
    return res;
  }
  static string decoding(string str){
    return "";
  }
};

NAME_SPACE_END()

#endif //!_UTIL_
