#ifndef _UTIL_TIMER_H
#define _UTIL_TIMER_H

#include <ctime>
#include <utility>
#include <string>
#include <thread>
#include <atomic>
#include <functional>

namespace myUtil {

template<typename R, typename... Args>
class Timer{
    using Callback = std::function<R(Args...)>;
public:
    explicit Timer(const std::string& name) : _name(name) {}
    template<typename Class>
    void setInterval(R (Class::*mFunc)(Args...), Class* object, long interval = 100) {
        _callback = [mFunc, object](Args... args) -> R {
            return (object->*mFunc)(std::forward<Args>(args)...);
        };
        _inter = interval;
        _loop = interval != 0;
    }
    void setInterval(R (*lpFunc)(Args...), long interval = 100) {
        _callback = [lpFunc](Args... args) -> R {
            return lpFunc(std::forward<Args>(args)...);
        };
        _inter = interval;
        _loop = interval != 0;
    }
    //开始运行 使用的是function
    void start(Args... args) {
        _work = std::thread(&Timer::innerThread, this, std::forward<Args>(args)...);
        _work.detach();
    }
    //结束运行
    void stop() {
        printf("end");
        _loop.store(false);
    }
private:
    void innerThread(Args... args) {
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(_inter));
            auto now = std::chrono::steady_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()).count();
            printf("tick at %lld ms\n", (long long)ms);
            _callback(std::forward<Args>(args)...);
        } while(_loop);
    }

private:
    std::string _name;
    std::thread _work;
    //false运行一次 true一直运行
    std::atomic_bool _loop{false};
    long _inter{100};
    Callback _callback;
};

} // namespace myUtil
#endif // !_UTIL_TIMER_H
