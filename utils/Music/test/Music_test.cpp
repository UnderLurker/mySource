#include <fstream>
#include <iostream>

#include "mp3.h"
#include "logger.h"
using namespace std;
using namespace myUtil;

void print(int number, const string& content) {
    int len = number;
    while(len--){
        CLog::push(content + string(1, (char)(number - len + '0')) + "\n");
    }
}

int main(){
    CLog::filePath = "E:/code/mySource/";
//    MP3 a("../../../Sounds/2.mp3");
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
    this_thread::sleep_for(chrono::seconds(1));
    CLog::end();
    Work.join();
    return 0;
}