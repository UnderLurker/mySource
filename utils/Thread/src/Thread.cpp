#include "Thread.h"
#include "Util.h"
#include <mutex>
#include <condition_variable>

NAME_SPACE_START(myUtil)

void ThreadPool::pushThread(ThreadPoolFunc func,void* args,int argsLen){
    unique_lock<mutex> lock(_threadPoolMutex);
    if(_threadPool.size()>=_threadMaxSize) return;
    ThreadPoolFuncAndArgs tpFA(func,args,argsLen);
    _threadPool.emplace_back(tpFA);
    _cv.notify_one();
}


NAME_SPACE_END()
