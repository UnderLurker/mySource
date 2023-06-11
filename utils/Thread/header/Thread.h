#ifndef _MY_THREAD__
#define _MY_THREAD_

#include "Util.h"
#include <deque>
#include <functional>
#include <list>
#include <mutex>
using namespace std;
NAME_SPACE_START(myUtil)

typedef void(*ThreadPoolFunction)();

class ThreadPool{
public:
    ThreadPool(){}
    ThreadPool(int PoolSize):_poolSize(PoolSize){}
    ~ThreadPool(){}
protected:
    void join(ThreadPoolFunction func){
        // lock_guard<mutex> lock(_threadPoolMutex);
        func();
    }
private:
    list<ThreadPoolFunction> _pool;
    // mutex _threadPoolMutex;
    int _curSize{0};
    int _poolSize{0};
};

NAME_SPACE_END()

#endif //!_MY_THREAD_