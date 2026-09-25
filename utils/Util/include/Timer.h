#ifndef _UTIL_TIMER_H
#define _UTIL_TIMER_H

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <string>
#include <thread>
#include <utility>

namespace myUtil {

template<typename R, typename... Args>
class Timer {
public:
    struct Invoke {
        virtual R invoke(Args...) const = 0;
    };
    template<typename = void>
    struct InvokeImpl : public Invoke {
        R (*func)(Args...);
        template<typename F>
        InvokeImpl(F&& f)
            : func(std::forward<F>(f)) {}
        R invoke(Args... args) const {
            assert(func != nullptr);
            return func(args...);
        }
    };
    template<typename T>
    struct InvokeImpl<R (T::*)(Args...)> : public Invoke {
        T* obj;
        R (T::*func)(Args...);
        template<typename F>
        InvokeImpl(F&& f, T* object)
            : func(std::forward<F>(f)), obj(object) {}
        R invoke(Args... args) const {
            assert(func != nullptr && obj != nullptr);
            return (obj->*func)(args...);
        }
    };

public:
    explicit Timer(const std::string& name)
        : _name(name) {}
    explicit Timer(Timer&& o) {
        _invoke = std::move(o._invoke);
        _inter  = o._inter;
        _loop   = o._loop;
        _name   = o._name;
    }
    virtual ~Timer() = default;
    template<typename T>
    void setInterval(R (T::*func)(Args...), T* object, long interval = 100) {
        _invoke.reset();
        _invoke = std::make_unique<InvokeImpl<R (T::*)(Args...)>>(func, object);
        _inter  = interval;
        _loop   = interval != 0;
    }

    void setInterval(R (*func)(Args...), long interval = 100) {
        _invoke.reset();
        _invoke = std::make_unique<InvokeImpl<R (*)(Args...)>>(func);
        _inter  = interval;
        _loop   = interval != 0;
    }
    // 开始运行 使用的是function
    void start(Args... args) {
        _work = std::thread(&Timer::innerThread, this, args...);
        _work.detach();
    }
    // 结束运行
    void stop() { _loop.store(false); }
    Timer& operator=(Timer&& o) {
        _invoke = std::move(o._invoke);
        _inter  = o._inter;
        _loop   = o._loop;
        _name   = o._name;
        return *this;
    }

private:
    void innerThread(Args... args) {
        do {
            std::unique_lock<std::mutex> guard(_mutex);
            _cv.wait_for(guard, std::chrono::milliseconds(_inter), [this]() { return !_loop; });
            auto now = std::chrono::steady_clock::now();
            auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            printf("now : %d\n", ms);
            _invoke->invoke(args...);
        } while (_loop);
    }
    Timer(const Timer&)            = delete;
    Timer& operator=(const Timer&) = delete;

private:
    std::string _name;
    std::thread _work;
    // false运行一次 true一直运行
    std::atomic_bool _loop {false};
    std::mutex _mutex;
    std::condition_variable _cv;
    long _inter {100};
    std::unique_ptr<Invoke> _invoke;
};

} // namespace myUtil
#endif // !_UTIL_TIMER_H
