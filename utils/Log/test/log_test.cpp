//
// Created by XW on 2024/3/22.
//
#include <iostream>

#include "logger.h"
using namespace myUtil;

CLOG_DECLARE_1(print, "E:/code/mySource/log/")

void print(int number, const char* content) {
    int len = number;
    while(len--){
        LogDebug(print, "%s%d", content, len);
        LogInfo(print, "%s%d", content, len);
        LogWarn(print, "%s%d", content, len);
        LogError(print, "%s%d", content, len);
    }
}

void clogTest() {
    CLOG_TYPE(print, ERROR)
    thread th(print, 5, "this is 0/");
    thread th1(print, 5, "this is 1/");
    thread th2(print, 5, "this is 2/");
    thread th3(print, 5, "this is 3/");
    thread th4(print, 5, "this is 4/");
    thread th5(print, 5, "this is 5/");
    thread th6(print, 5, "this is 6/");
    thread th7(print, 5, "this is 7/");
    thread th8(print, 5, "this is 8/");
    thread th9(print, 5, "this is 9/");
    th.join();
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
    th6.join();
    th7.join();
    th8.join();
    th9.join();
    stringstream ss;
    ss << this_thread::get_id();
}

int main(){
    clogTest();
    CLOG_END(print)
    return 0;
}

