#include "Thread.h"
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
using namespace std;
using namespace myUtil;

// condition_variable cv;
mutex mtx;

void print(void* ch){
    int t=(int)*((int*)ch);
    for(int i=0;i<10;i++){
        cout<<t<<":"<<i<<" ";
    }
    cout<<endl;
}

int main(){
    ThreadPool t(10,4);
    for(int i=0;i<8;i++){
        // cout<<"submit task: "<<i<<endl;
        int temp=i;
        t.pushThread(print, (void*)&temp, sizeof(i));
    }
    this_thread::sleep_for(chrono::seconds(3));
    return 0;
}
