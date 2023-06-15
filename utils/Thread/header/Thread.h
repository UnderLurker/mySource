#ifndef _MY_THREAD__
#define _MY_THREAD_

#include "Util.h"
#include "Singleton.h"
#include <chrono>
#include <deque>
#include <functional>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string.h>
#include <iostream>
#include <thread>
#include <vector>

NAME_SPACE_START(myUtil)
using namespace std;

typedef void(*ThreadPoolFunc)(void*);

class ThreadPoolFuncAndArgs{
public:
    ThreadPoolFunc _func;
    void* _args{nullptr};
    int _argsLen{0};
    ThreadPoolFuncAndArgs(){}
    ThreadPoolFuncAndArgs(const ThreadPoolFuncAndArgs& val):
        _func(val._func),_args(val._args),_argsLen(val._argsLen){}
    ThreadPoolFuncAndArgs(ThreadPoolFunc func,void* args,int argsLen):
        _func(func),_args(args),_argsLen(argsLen){}
};
class ThreadPool{
    class Worker{
        friend ThreadPool;
        // thread::id _threadID;
        int _threadID{0};
        ThreadPool *_pool{nullptr};
        ThreadPoolFuncAndArgs _funcAndArgs;
        bool hasTask{false};
    public:
        Worker(){}
        Worker(const Worker&) = default;
        Worker(ThreadPool *pool,int threadID):_pool(pool){
            // this->_threadID=this_thread::get_id();
            this->_threadID=threadID;
        }
        Worker(ThreadPool *pool,int threadID,ThreadPoolFuncAndArgs funcAndArgs):_pool(pool),_funcAndArgs(funcAndArgs){
            // this->_threadID=this_thread::get_id();
            this->_threadID=threadID;
        }
        void operator()(){
            cout<<"id:"<<this->_threadID<<" enter"<<endl;
            while(!_pool->_terminate){
                {
                    unique_lock<mutex> poolLock(_pool->_threadPoolMutex);
                    cout<<"id:"<<this->_threadID<<" waiting"<<endl;

                    _pool->_cv.wait(poolLock,[&]{
                        if(_pool->_terminate) return true;
                        return !_pool->_threadPool.empty();
                    });
                    
                    if(!_pool->_threadPool.empty()){
                        _funcAndArgs=_pool->_threadPool.front();
                        _pool->_threadPool.pop_front();
                        hasTask=true;
                    }
                }

                if(hasTask){
                    cout<<"id:"<<this->_threadID<<" execute..."<<endl;
                    _funcAndArgs._func(_funcAndArgs._args);
                    cout<<"id:"<<this->_threadID<<" done!!"<<endl;
                    hasTask=false;
                }

            }
            cout<<"id:"<<this->_threadID<<" exit"<<endl;
        }
    };
public:
    ThreadPool()=delete;
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(int threadMaxSize,int workerMaxSize):
        _threadMaxSize(threadMaxSize),_workerMaxSize(workerMaxSize){
        _worker.resize(workerMaxSize);
        for(int i=0;i<workerMaxSize;i++){
            _worker.at(i)=thread(Worker(this,i));
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
    ~ThreadPool(){
        this->_terminate=true;
        _cv.notify_all();
        for(int i=0;i<_workerMaxSize;i++){
            if(_worker[i].joinable()) _worker[i].join();
        }

    }
    ThreadPool& operator=(const ThreadPool&) = delete;

    void pushThread(ThreadPoolFunc func,void* args,int argsLen);
private:
    mutex _threadPoolMutex;
    condition_variable _cv;
    list<ThreadPoolFuncAndArgs> _threadPool;//存放任务队列
    vector<thread> _worker;//真正工作的线程
    int _threadMaxSize{0};//对应_threadPool
    int _workerMaxSize{0};
    bool _terminate{false};
};

NAME_SPACE_END()

#endif //!_MY_THREAD_
