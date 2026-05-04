#include "Thread.h"
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
using namespace std;

// condition_variable cv;
mutex mtx;

void print(int ch){
    int t=ch;
    for(int i=0;i<10;i++){
        cout<<t<<":"<<i<<" ";
    }
    cout<<endl;
}
void print1(double ch){
    for(int i=0;i<10;i++){
        cout<<ch<<":"<<i<<" ";
    }
    cout<<endl;
}

int main(){
    myUtil::ThreadPool t(10,4);
    for(int i=0;i<8;i++){
        // cout<<"submit task: "<<i<<endl;
        int temp=i;
        t.push(print, temp);
    }
    for(int i=0;i<8;i++){
        t.push(print1, i + 0.1);
    }
    // this_thread::sleep_for(chrono::seconds(3));
    return 0;
}
