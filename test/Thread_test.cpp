#include "Thread.h"
#include <iostream>
#include <thread>
using namespace std;
using namespace myUtil;
void print(){
    cout<<"abdfsf"<<endl;
}
int main(){
    // ThreadPool pool;
    thread a(print);
    a.join();
    return 0;
}