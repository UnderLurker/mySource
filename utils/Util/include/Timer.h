#ifndef _UTIL_TIMER_H
#define _UTIL_TIMER_H

#include <atomic>
#include <condition_variable>
#include <string>
#include <thread>
#include <utility>

#include "include/logger_wrapper.h"

namespace myUtil {

constexpr int32_t INTERVAL_TIME = 100;

template<typename R, typename... Args>
class Timer {
public:
    struct Invoke {
        virtual ~Invoke()               = default;
        virtual R invoke(Args...) const = 0;
    };
    template<typename F>
    struct InvokeImpl;

    template<typename... A>
    struct InvokeImpl<R (*)(A...)> : public Invoke {
        R (*func)(A...);
        template<typename F>
        InvokeImpl(F&& f)
            : func(std::forward<F>(f)) {}
        R invoke(A... args) const override {
            if (func == nullptr) {
                if constexpr (std::is_void_v<R>) return;
                else return R();
            }
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
        R invoke(Args... args) const override {
            if (obj == nullptr || func == nullptr) {
                if constexpr (std::is_void_v<R>) return;
                else return R();
            }
            return (obj->*func)(args...);
        }
    };
    template<typename T>
    struct InvokeImpl<R (T::*)(Args...) const> : public Invoke {
        const T* obj;
        R (T::*func)(Args...) const;
        template<typename F>
        InvokeImpl(F&& f, T* object)
            : func(std::forward<F>(f)), obj(object) {}
        R invoke(Args... args) const override {
            if (obj == nullptr || func == nullptr) {
                if constexpr (std::is_void_v<R>) return;
                else return R();
            }
            return (obj->*func)(args...);
        }
    };
    template<typename T>
    struct InvokeImpl : public Invoke {
        T func;
        template<typename F>
        InvokeImpl(F&& f)
            : func(std::forward<F>(f)) {}
        R invoke(Args... args) const override {
            if constexpr (std::is_void_v<R>) func(args...);
            else return func(args...);
        }
    };

public:
    explicit Timer(const std::string& name)
        : _name(name) {}
    virtual ~Timer() { stop(); }
    template<typename F, typename T>
    void setInterval(F&& func, T* object, long interval = INTERVAL_TIME) {
        _invoke.reset();
        _invoke = std::make_unique<InvokeImpl<std::decay_t<F>>>(std::forward<F>(func), object);
        _inter  = interval;
        _loop   = interval != 0;
    }
    template<typename F>
    void setInterval(F&& func, long interval = INTERVAL_TIME) {
        _invoke.reset();
        _invoke = std::make_unique<InvokeImpl<std::decay_t<F>>>(std::forward<F>(func));
        _inter  = interval;
        _loop   = interval != 0;
    }
    // 先调用setInterval
    void start(Args... args) {
        if (_run.load()) {
            LOGW("had a thread.");
            return;
        }
        _work = std::thread(&Timer::innerThread, this, args...);
        _run.store(true);
    }
    // 结束运行
    void stop() {
        _loop.store(false);
        notifyThread();
    }

private:
    void innerThread(Args... args) {
        do {
            std::unique_lock<std::mutex> guard(_mutex);
            _cv.wait_for(guard, std::chrono::milliseconds(_inter), [&]() { return !_loop; });
            if (!_loop.load()) break;
            guard.unlock();
            if (_invoke) {
                _invoke->invoke(args...);
            } else {
                LOGW("_invoke is nullptr!!!");
            }
        } while (_loop);
    }
    void notifyThread() {
        _cv.notify_one();
        if (_work.joinable()) {
            _work.join();
            _run.store(false);
        }
    }
    Timer(const Timer&)            = delete;
    Timer(Timer&& o)               = delete;
    Timer& operator=(Timer&& o)    = delete;
    Timer& operator=(const Timer&) = delete;

private:
    std::string _name;
    std::thread _work;
    // false运行一次 true一直运行, 若interval间隔为0则自动设为false
    std::atomic_bool _loop {false};
    std::atomic_bool _run {false};
    std::mutex _mutex;
    std::condition_variable _cv;
    long _inter {INTERVAL_TIME};
    std::unique_ptr<Invoke> _invoke;
};

} // namespace myUtil
#endif // !_UTIL_TIMER_H
