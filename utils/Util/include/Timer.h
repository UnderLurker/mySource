#ifndef _UTIL_TIMER_H
#define _UTIL_TIMER_H

#include <ctime>
#include <utility>
#include <string>
#include <thread>
#include <atomic>
#include <algorithm>
#include <condition_variable>

namespace myUtil {

template<typename R, typename... Args>
class Timer{
    using ComFunc = R (*)(Args...);
    struct MemFuncBase {
        virtual R operator()(Args...) = 0;
    };
    template<typename T>
    struct MemFunc : public MemFuncBase {
    private:
        using Callback = R (T::*)(Args...);
    public:
        T* obj;
        Callback lpFunc;
        MemFunc(T* o, Callback func) : obj(o), lpFunc(func) {}
        R operator()(Args... args) {
            return (obj->*lpFunc)(std::forward<Args>(args)...);
        }
    };
    union Func {
        ComFunc lpFunc;
        MemFuncBase* lpMemFunc;
    };
    enum Type {
        COMMON,
        MEMBER,
    };
public:
    explicit Timer(const std::string& name) : _name(name) {}
    virtual ~Timer() {
        if (_type == MEMBER && _func.lpMemFunc) {
            delete _func.lpMemFunc;
        }
    }
    template<typename T>
    void setInterval(R (T::*mFunc)(Args...), T* object, long interval = 100) {
        _func.lpMemFunc = new MemFunc<T>(object, mFunc);
        _inter = interval;
        _loop = interval != 0;
        _type = MEMBER;
    }
    void setInterval(R (*lpFunc)(Args...), long interval = 100) {
        _func.lpFunc = lpFunc;
        _inter = interval;
        _loop = interval != 0;
        _type = COMMON;
    }
    //开始运行 使用的是function
    void start(Args... args) {
        _work = std::thread(&Timer::innerThread, this, std::forward<Args>(args)...);
        _work.detach();
    }
    //结束运行
    void stop() {
        _loop.store(false);
    }
private:
    void innerThread(Args... args) {
        do {
            std::unique_lock<std::mutex> guard(_mutex);
            _cv.wait_for(guard, std::chrono::milliseconds(_inter), [this](){
                return !_loop;
            });
            auto now = std::chrono::steady_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()).count();
            if (_type == COMMON) {
                (_func.lpFunc)(args...);
            } else if (_type == MEMBER) {
                (*_func.lpMemFunc)(args...);
            }
        } while(_loop);
    }

private:
    std::string _name;
    std::thread _work;
    //false运行一次 true一直运行
    std::atomic_bool _loop{false};
    std::mutex _mutex;
    std::condition_variable _cv;
    long _inter{100};
    Func _func;
    Type _type;
};

} // namespace myUtil
#endif // !_UTIL_TIMER_H
