#ifndef _SPSC_DEQUE_H
#define _SPSC_DEQUE_H
#include <atomic>
#include <iostream>
#include <optional>

template<typename T>
class SPSCQueue {
public:
    explicit SPSCQueue(int32_t length) : _rawPtr(new T[length + 1]), _length(length) {
        _readIndex.store(0);
        _writeIndex.store(0);
    }

    bool push(T value) {
        auto writeIndex = _writeIndex.load(std::memory_order_relaxed);
        int32_t next = (writeIndex + 1) % _length;
        if (next == _readIndex.load(std::memory_order_acquire)) {
            // std::cout << "SPSC is filled." << std::endl;
            return false;
        }
        _rawPtr[writeIndex] = value;
        _writeIndex.store(next, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        auto readIndex = _readIndex.load(std::memory_order_relaxed);
        if (readIndex == _writeIndex.load(std::memory_order_acquire)) {
            // std::cout << "SPSC is emptyed." << std::endl;
            return std::nullopt;
        }
        T res = _rawPtr[readIndex];
        int32_t next = (readIndex + 1) % _length;
        _readIndex.store(next, std::memory_order_release);
        return res;
    }

private:
    std::atomic<int32_t> _readIndex;
    std::atomic<int32_t> _writeIndex;
    T* _rawPtr{nullptr};
    int32_t _length;
};

#endif // !_SPSC_DEQUE_H
