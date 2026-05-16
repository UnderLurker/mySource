#ifndef _LOCK_FREE_QUEUE_H
#define _LOCK_FREE_QUEUE_H

#include <atomic>
#include <optional>
#include <iostream>

template<typename T>
class LockFreeQueue {
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node(T value, Node* nextPtr) : data(value), next(nextPtr) {}
    };
public:
    explicit LockFreeQueue(int32_t maxLength) : _maxLength(maxLength), _curLength(0) {}

    bool push(T val) {
        Node* nullNode = nullptr;
        Node* newNode = new Node(std::move(val), nullptr);
        while(true) {
            if (_curLength.load(std::memory_order_acquire) >= _maxLength) {
                // std::cout << "LockFreeQueue is filled!!!" << std::endl;
                delete newNode;
                return false;
            }
            Node* oldTail = _tail.load(std::memory_order_acquire);
            if (!oldTail) {
                if (_head.compare_exchange_weak(nullNode, newNode, std::memory_order_release, std::memory_order_relaxed)) {
                    _tail.store(newNode, std::memory_order_release);
                    _curLength.fetch_add(1, std::memory_order_release);
                    return true;
                }
                continue;
            }
            Node* lastTail = oldTail->next.load(std::memory_order_relaxed);
            if (lastTail) {
                _tail.compare_exchange_weak(oldTail, lastTail, std::memory_order_release, std::memory_order_acquire);
                continue;
            }
            if (oldTail->next.compare_exchange_weak(lastTail, newNode, std::memory_order_release, std::memory_order_relaxed)) {
                _tail.compare_exchange_weak(oldTail, newNode, std::memory_order_release, std::memory_order_acquire);
                break;
            }
        }
        _curLength.fetch_add(1, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        Node* nextHead = nullptr;
        while(true) {
            if (_curLength.load(std::memory_order_acquire) <= 0) {
                // std::cout << "LockFreeQueue is empty!!!" << std::endl;
                return std::nullopt;
            }
            auto oldHead = _head.load(std::memory_order_acquire);
            if (!oldHead) return std::nullopt;
            auto nextHead = oldHead->next.load(std::memory_order_acquire);
            if (_head.compare_exchange_weak(oldHead, nextHead, std::memory_order_release, std::memory_order_acquire)) {
                T res = std::move(oldHead->data);
                delete oldHead;
                _curLength.fetch_sub(1, std::memory_order_release);
                return res;
            }
        }
    }

    bool empty() const {
        return !_curLength.load(std::memory_order_acquire);
    }

    bool isFill() const {
        return _curLength.load(std::memory_order_acquire) == _maxLength;
    }

private:
    int32_t _maxLength{0};
    std::atomic<int32_t> _curLength{0};
    std::atomic<Node*> _head{nullptr};
    std::atomic<Node*> _tail{nullptr};

};

#endif // !_LOCK_FREE_QUEUE_H