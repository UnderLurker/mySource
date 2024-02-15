//
// Created by 常笑男 on 2024/2/14.
//

#ifndef _HEAP_H
#define _HEAP_H

#include <cassert>

#include "Util.h"

NAME_SPACE_START(myUtil)

//#define HEAP_DEBUG
#define HEAP_PRINT_DEBUG
#define AMPLIFY_NUM 2

template<typename valueType>
struct Iterator {
    using pointer = valueType*;
    using reference = valueType&;

    reference operator*() { return *_ptr; }
    pointer operator->() { return _ptr; }

    Iterator& operator++() {
        if (_ptr) _ptr = _ptr + 1;
        else _ptr = nullptr;
        return *this;
    }
    Iterator& operator++(int) {
        if (_ptr) _ptr = _ptr + 1;
        else _ptr = nullptr;
        return *this;
    }
    bool operator==(const Iterator<valueType>& obj) const { return _ptr == obj._ptr; }
    bool operator!=(const Iterator<valueType>& obj) const { return _ptr != obj._ptr; }

    pointer _ptr;
};

template<typename valueType>
struct ReverseIterator {
    using pointer = valueType*;
    using reference = valueType&;

    reference operator*() { return *_ptr; }
    pointer operator->() { return _ptr; }

    ReverseIterator& operator++() {
        if (_ptr) _ptr = _ptr - 1;
        else _ptr = nullptr;
        return *this;
    }
    ReverseIterator& operator++(int) {
        if (_ptr) _ptr = _ptr - 1;
        else _ptr = nullptr;
        return *this;
    }
    bool operator==(const ReverseIterator<valueType>& obj) const { return _ptr == obj._ptr; }
    bool operator!=(const ReverseIterator<valueType>& obj) const { return _ptr != obj._ptr; }

    pointer _ptr;
};

/**
 * 堆
 * @tparam T
 * @tparam Compare less<T>, greater<T> or custom
 */
template <typename T, typename Compare = less<T>>
class heap {
public:
    using iterator = Iterator<T>;
    using const_iterator = const Iterator<T>;
    using reverse_iterator = ReverseIterator<T>;
    using const_reverse_iterator = const ReverseIterator<T>;
public:
    heap() = default;
    heap(const heap& obj);
    heap(uint32_t size, T val);
    heap(T* begin, T* end, uint32_t length = -1);
    ~heap() { if (_values) delete[] _values; }

    heap& operator=(const heap& obj);

    heap(const heap&&) = delete;
    heap& operator=(const heap&&) = delete;

    uint32_t size() const { return _curLength; }
    uint32_t capacity() const { return _maxLength; }
    void resize(uint32_t size);
    bool empty() const;
    T front() const;
    T back() const;

    void push_back(const T& val);
    void pop_back();
    void erase(uint32_t pos);
    void erase(iterator iter);
    void erase(reverse_iterator iter);
    iterator begin() const;
    iterator end() const;
    reverse_iterator rbegin() const;
    reverse_iterator rend() const;
//    const_iterator cbegin() const;
//    const_iterator cend() const;
//    const_reverse_iterator crbegin() const;
//    const_reverse_iterator crend() const;
    void swap(T& lPos, T& rPos);

    template<typename... Args>
    void emplace_back(Args&&... args);
#ifdef HEAP_PRINT_DEBUG
    void print(const char* info = nullptr);
#endif
private:
    // [0, inf)
    void balance(uint32_t pos);
    void balanceAll();

private:
    T* _values { nullptr };
    uint32_t _curLength { 0 };
    uint32_t _maxLength { 0 };
};

NAME_SPACE_END()

#include "heap.inl"
#endif // !_HEAP_H
