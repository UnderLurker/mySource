#include <condition_variable>
#include <mutex>
#define _CRT_SECURE_NO_WARNINGS
#include "Util.h"
#include <cstring>
#include <fstream>
#include <functional>
#include <ios>
#include <streambuf>
#include <string>
#include <thread>
#include <time.h>

NAME_SPACE_START(myUtil)

std::string getFile(const std::string& filePath)
{
    std::string content = "";
    std::fstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.fail()) {
        while (!file.eof()) {
            char ch;
            file.get(ch);
            content.push_back(ch);
        }
    }
    file.close();
    return content;
}

Timer::Timer(){
    startTime=clock();
}

Timer::Timer(void* lpFunc){
    startTime=clock();
    this->lpFunc=lpFunc;
}

std::string Timer::getRunTime(const std::string &format){
    curTime=clock();
    clock_t temp=curTime-startTime;
    if(format=="millisecond") return std::to_string(temp);
    else if(format=="second") return std::to_string(temp/CLOCKS_PER_SEC);
    return std::to_string(temp);
}

void Timer::setInterval(void *lpFunc,long interval){
    runFlag=true;
    this->lpFunc=lpFunc;
    this->inter=interval;
}

template<typename classType, typename ...Args>
void Timer::start(Args... args){
    typedef std::function<void(classType,Args...)> classFunc;
    classFunc* func=(classFunc*)lpFunc;
    while(runFlag){
        // Sleep(inter);
        // std::thread th([](Args... args){
        //     (*func)(args...);
        // },args...);
        // th.join();
    }
}

void Timer::end(){
    runFlag=false;
}

vector<string> Split(const string& source, char ch){
    vector<string> res;
    size_t pre=0,pos=0;
    while((pos=source.find(ch,pre))!=string::npos){
        string str(source.begin()+pre,source.begin()+pos);
        res.push_back(str);
        pre=pos+1;
    }
    if(pre!=0) res.push_back(source.substr(pre));
    return res;
}
vector<string> Split(const char* source,size_t len,char ch){
    string str(source);
    return Split(str,ch);
}
vector<wstring> Split(const wstring& source, wchar_t ch){
    vector<wstring> res;
    size_t pre=0,pos=0;
    while((pos=source.find(ch,pre))!=wstring::npos){
        wstring str(source.begin()+pre,source.begin()+pos);
        res.push_back(str);
        pre=pos+1;
    }
    if(pre!=0) res.push_back(source.substr(pre));
    return res;
}
vector<wstring> Split(const wchar_t* source,size_t len,wchar_t ch){
    wstring str(source);
    return Split(str,ch);
}

int ProgressBar::Logic::barLen = 100;
int ProgressBar::Logic::curLen = 0;
thread ProgressBar::run;
string ProgressBar::Logic::str = "";
bool ProgressBar::_pause = false;
char ProgressBar::Logic::ch = '=';
condition_variable ProgressBar::Logic::_cv;
mutex ProgressBar::barMutex;

NAME_SPACE_END()
