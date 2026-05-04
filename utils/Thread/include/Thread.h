#ifndef _MY_THREAD__
#define _MY_THREAD_

#include <future>
#include <functional>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string.h>
#include <iostream>
#include <thread>
#include <vector>

namespace myUtil {
using namespace std;

class ThreadPoolPackaged {
public:
    std::packaged_task<void()> _task;
    ThreadPoolPackaged() = default;
    ThreadPoolPackaged(std::packaged_task<void()>&& task) : _task(move(task)) {}
};
class ThreadPool{
    class Worker{
        friend ThreadPool;
        // thread::id _threadID;
        int _threadID{0};
        ThreadPool *_pool{nullptr};
        bool hasTask{false};
    public:
        Worker(){}
        Worker(const Worker&) = default;
        Worker(ThreadPool *pool,int threadID):_pool(pool){
            // this->_threadID=this_thread::get_id();
            this->_threadID=threadID;
        }
        // Worker(ThreadPool *pool,int threadID,ThreadPoolPackaged funcAndArgs):_pool(pool),_funcAndArgs(funcAndArgs){
        //     // this->_threadID=this_thread::get_id();
        //     this->_threadID=threadID;
        // }
        void operator()() {
            cout << "id:" << this->_threadID << " enter" << endl;
            while (!_pool->_terminate) {
                ThreadPoolPackaged package;
                {
                    unique_lock<mutex> poolLock(_pool->_threadPoolMutex);
                    cout << "id:" << this->_threadID << " waiting" << endl;

                    _pool->_cv.wait(poolLock, [&] {
                        return _pool->_terminate || !_pool->_threadPool.empty();
                    });

                    if (!_pool->_threadPool.empty()) {
                        package = move(_pool->_threadPool.front());
                        _pool->_threadPool.pop_front();
                        hasTask = true;
                    }
                }

                if (hasTask) {
                    cout << "id:" << this->_threadID << " execute..." << endl;
                    package._task();
                    cout << "id:" << this->_threadID << " done!!" << endl;
                    hasTask = false;
                }
            }
            cout << "id:" << this->_threadID << " exit" << endl;
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

    template<typename F, typename... Args>
    void push(F&& func, Args&&... args) {
        using return_type = invoke_result_t<F, Args...>;
        unique_lock<mutex> lock(_threadPoolMutex);
        auto task = std::make_shared<packaged_task<return_type()>>(bind(forward<F>(func), forward<Args>(args)...));
        _threadPool.push_back({std::packaged_task<void()>([task = move(task)]() { (*task)(); })});
        lock.unlock();
        _cv.notify_one();
    }
private:
    mutex _threadPoolMutex;
    condition_variable _cv;
    list<ThreadPoolPackaged> _threadPool;//存放任务队列
    vector<thread> _worker;//真正工作的线程
    int _threadMaxSize{0};//对应_threadPool
    int _workerMaxSize{0};
    bool _terminate{false};
};
}

#endif //!_MY_THREAD_
