// #include "Thread.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
using namespace std;
// using namespace myUtil;
mutex mtx;
void print(){
    unique_lock<mutex> lock(mtx);
    for(int i=0;i<100;i++){
        cout<<i<<" ";
    }
}

struct A{
    void operator()(int a){
        for(int i=0;i<100;i++){
            cout<<a<<":"<<i<<" ";
        }
    }
};
int main(){
    // ThreadPool pool;
    vector<thread> t;
    t.resize(3);
    for(int i=0;i<3;i++){
        t.at(i)=thread((A()),i);
    }
    this_thread::sleep_for(chrono::seconds(3));
    return 0;
}