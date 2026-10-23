#ifndef _LOGGER_WRAPPER_H
#define _LOGGER_WRAPPER_H

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <string>
#include <utility>

#ifdef UTIL_DEBUG
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>
#ifdef LOGGER_CLIENT
#include "client/include/log_client.h"
#endif
#endif

namespace myUtil {

#ifdef UTIL_DEBUG

enum LogLevel : uint8_t {
    ERROR   = 0,
    DEBUG   = 1,
    WARNING = 2,
    INFO    = 3,
};

inline const char* ToStrLevel(LogLevel level) {
    switch (level) {
        case ERROR:
            return "[ERROR] ";
        case DEBUG:
            return "[DEBUG] ";
        case WARNING:
            return "[WARNING] ";
        case INFO:
            return "[INFO] ";
    }
    return "[UNKNOWN] ";
}

inline std::string CurrTime() {
    char tmp[64];
    std::time_t ptime = std::time(nullptr);
    std::tm tm_buf {};
#if defined(_WIN32)
    localtime_s(&tm_buf, &ptime);
#else
    localtime_r(&ptime, &tm_buf);
#endif
    std::strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S ", &tm_buf);
    return std::string(tmp);
}

// 线程安全、无固定上限栈缓冲的 vsnprintf 追加
inline void FormatVAppend(std::string& out, const char* fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = std::vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);
    if (needed <= 0) return;
    const size_t old = out.size();
    out.resize(old + static_cast<size_t>(needed) + 1);
    std::vsnprintf(&out[old], static_cast<size_t>(needed) + 1, fmt, args);
    out.resize(old + static_cast<size_t>(needed));
}

inline std::string
    BuildPrefix(LogLevel level, const std::string& file, uint32_t line, const std::string& function) {
    std::string out;
    out.clear();
    out.reserve(128);
#ifdef LOG_TIME
    out += CurrTime();
#endif
    out += ToStrLevel(level);
#ifdef LOG_FILE_PATH
    size_t pos = file.rfind('/');
    if (pos == std::string::npos) { pos = file.rfind('\\'); }
    if (pos != std::string::npos) {
        out.append(file, pos + 1, std::string::npos);
    } else {
        out += file;
    }
    out += ':';
    out += std::to_string(line);
    out += ' ';
#endif
    out += function;
    out += ' ';
    return out;
}

// ====================== MPMC 有界无锁环形队列 ======================
// Vyukov 算法：
//   Cell::seq == pos     → 该槽位可被生产者 pos 写
//   Cell::seq == pos + 1 → 该槽位可被消费者 pos 读
//   消费者读完把 seq 置为 pos + capacity，供下一轮复用
template<typename T>
class BoundedMpmcRing {
public:
    explicit BoundedMpmcRing(size_t capacity_pow2)
        : capacity_(capacity_pow2), mask_(capacity_pow2 - 1), buffer_(capacity_pow2) {
        for (size_t i = 0; i < capacity_; ++i) {
            buffer_[i].seq.store(i, std::memory_order_relaxed);
        }
    }

    BoundedMpmcRing(const BoundedMpmcRing&)            = delete;
    BoundedMpmcRing& operator=(const BoundedMpmcRing&) = delete;

    // 多生产者入队：成功返回 true；队列满返回 false
    bool Push(T&& item) {
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
        for (;;) {
            Cell& cell               = buffer_[pos & mask_];
            const size_t seq         = cell.seq.load(std::memory_order_acquire);
            const std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos);
            if (diff == 0) {
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed,
                                                       std::memory_order_relaxed)) {
                    cell.data = std::move(item);
                    cell.seq.store(pos + 1, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false; // 队列满
            } else {
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }
    }

    // 单消费者出队：成功返回 true；空返回 false
    bool Pop(T& out) {
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
        for (;;) {
            Cell& cell               = buffer_[pos & mask_];
            const size_t seq         = cell.seq.load(std::memory_order_acquire);
            const std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos + 1);
            if (diff == 0) {
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed,
                                                       std::memory_order_relaxed)) {
                    out = std::move(cell.data);
                    cell.data.clear();
                    cell.seq.store(pos + capacity_, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false; // 队列空
            } else {
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }
    }

private:
    struct Cell {
        std::atomic<size_t> seq {0};
        T data;
    };

    const size_t capacity_;
    const size_t mask_;
    std::vector<Cell> buffer_;
    alignas(64) std::atomic<size_t> enqueue_pos_ {0};
    alignas(64) std::atomic<size_t> dequeue_pos_ {0};
};

// ============ 异步日志队列（单后台消费线程 + MPMC 缓冲）============
class AsyncLogQueue {
public:
    static constexpr size_t kDefaultCapacity = 1u << 14; // 16384

    explicit AsyncLogQueue(size_t capacity = kDefaultCapacity)
        : ring_(RoundUpPow2(capacity)) {}

    ~AsyncLogQueue() { Stop(); }   // 自动停止并 join

    AsyncLogQueue(const AsyncLogQueue&) = delete;
    AsyncLogQueue& operator=(const AsyncLogQueue&) = delete;

    void Start() {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true,
                std::memory_order_acq_rel)) {
            return;
        }
        accepting_.store(true, std::memory_order_release);
        worker_ = std::thread([this] { Consume(); });   // 不 detach
    }

    // 幂等；由析构函数自动调用，也允许显式调用
    void Stop() noexcept {
        accepting_.store(false, std::memory_order_release);
        if (!running_.exchange(false, std::memory_order_acq_rel)) {
            return;   // 从未 Start
        }
        try {
            if (worker_.joinable()) {
                worker_.join();   // 等 Consume 排空并退出
            }
        } catch (...) {
            // join 失败（例如自身线程调 Stop）不能抛，析构必须 noexcept
        }
    }

    void Enqueue(std::string&& msg) {
        if (!accepting_.load(std::memory_order_acquire)) {
            dropped_.fetch_add(1, std::memory_order_relaxed);
            return;
        }
        if (!ring_.Push(std::move(msg))) {
            dropped_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    size_t Dropped() const {
        return dropped_.load(std::memory_order_relaxed);
    }

private:
    static size_t RoundUpPow2(size_t x) {
        size_t p = 1;
        while (p < x) p <<= 1;
        return p;
    }

    void Consume() noexcept {
        std::string msg;
        unsigned idle = 0;
        while (running_.load(std::memory_order_acquire)) {
            if (ring_.Pop(msg)) {
                try { Emit(std::move(msg)); } catch (...) {}
                idle = 0;
            } else if (++idle < 64) {
                std::this_thread::yield();
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                idle = 0;
            }
        }
        // 排空剩余（带超时兜底，防止生产者不停导致活锁）
        auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (ring_.Pop(msg)) {
            try { Emit(std::move(msg)); } catch (...) {}
            if (std::chrono::steady_clock::now() >= deadline) break;
        }
    }

    static void Emit(std::string&& msg) {
#ifdef LOGGER_CLIENT
        static auto client = LogClient::GetInstance();  // 引用，无新构造
        client->Send(std::move(msg));
#else
        msg += '\n';
        std::fwrite(msg.data(), 1, msg.size(), stdout);
        std::fflush(stdout);
#endif
    }

    BoundedMpmcRing<std::string> ring_;
    std::atomic<size_t> dropped_{0};
    std::atomic<bool>   accepting_{false};
    std::atomic<bool>   running_{false};
    std::thread         worker_;   // 不再 detach
};

// 故意用 new 创建，避免静态析构顺序问题（进程退出由 OS 回收）
inline AsyncLogQueue& GetLogQueue() {
    static AsyncLogQueue q;
    return q;
}

inline std::atomic<bool>& LogStartedFlag() {
    static std::atomic<bool> f{false};
    return f;
}

inline void EnsureLogWorkerStarted() {
    if (LogStartedFlag().load(std::memory_order_acquire)) return;

    static std::once_flag flag;
    std::call_once(flag, [] {
#ifdef LOGGER_CLIENT
        LogClient::GetInstance();   // ★ 关键：先构造 LogClient
#endif
        GetLogQueue().Start();      // 后构造 AsyncLogQueue
        LogStartedFlag().store(true, std::memory_order_release);
    });
}

inline void UtilPrint(LogLevel level,
                      const std::string& file,
                      uint32_t line,
                      const std::string& function,
                      const char* fmt,
                      ...) {
    EnsureLogWorkerStarted();

    std::string out = BuildPrefix(level, file, line, function);

    va_list args;
    va_start(args, fmt);
    FormatVAppend(out, fmt, args);
    va_end(args);

    GetLogQueue().Enqueue(std::move(out));
}

#define LOGE(fmt, ...)                                                                                        \
    do {                                                                                                      \
        ::myUtil::UtilPrint(::myUtil::LogLevel::ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
    } while (0)
#define LOGD(fmt, ...)                                                                                        \
    do {                                                                                                      \
        ::myUtil::UtilPrint(::myUtil::LogLevel::DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
    } while (0)
#define LOGW(fmt, ...)                                                                                          \
    do {                                                                                                        \
        ::myUtil::UtilPrint(::myUtil::LogLevel::WARNING, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
    } while (0)
#define LOGI(fmt, ...)                                                                                       \
    do {                                                                                                     \
        ::myUtil::UtilPrint(::myUtil::LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
    } while (0)

#else // UTIL_DEBUG

#define LOGE(fmt, ...) ((void)0)
#define LOGD(fmt, ...) ((void)0)
#define LOGW(fmt, ...) ((void)0)
#define LOGI(fmt, ...) ((void)0)

#endif // UTIL_DEBUG

} // namespace myUtil

#endif // !_LOGGER_WRAPPER_H
