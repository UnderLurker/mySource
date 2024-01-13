//
// Created by 常笑男 on 2024/1/7.
//

#ifndef MAIN_SINGLELINKLIST_H
#define MAIN_SINGLELINKLIST_H

#include <functional>

#include "Util.h"

NAME_SPACE_START(myUtil)

#define SINGLE_LINK_LIST_MAX_LENGTH 1024

template<typename T>
void DefaultDeleter(T& value)
{}

template<typename T>
class SingleLinkNode {
    typedef function<void(T&)> Deleter;

public:
    explicit SingleLinkNode(const T& value, SingleLinkNode<T>* next = nullptr,
                            Deleter deleter = DefaultDeleter<T>)
        : _data(value), _next(next), _deleter(deleter)
    {}
    ~SingleLinkNode()
    {
        _deleter(_data);
    }
    SingleLinkNode<T>* next() const { return _next; }
    void setNext(SingleLinkNode* next) { _next = next; }
    void insert(const T& value);
    T value() const { return _data; }
    void setValue(const T& value) { _data = value; }
private:
    T _data{ T() };
    Deleter _deleter { DefaultDeleter };
    SingleLinkNode<T>* _next { nullptr };
};
//
//     SingleLinkList struct
//     ┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐
//     │    1   │─►│    2   │─►│    3   │─►│    4   │─►│   end  │
//     └────────┘  └────────┘  └────────┘  └────────┘  └────────┘
//          ▲                                               │
//          └───────────────────────────────────────────────┘
//
template<typename T>
class SingleLinkList {
public:
    template<typename value_type>
    struct SingleLinkList_Iterator {
        using pointer = SingleLinkNode<value_type>*;
        using reference = SingleLinkNode<value_type>&;
        using iterator = SingleLinkNode<value_type>*;
        using const_iterator = const SingleLinkNode<value_type>*;

        reference operator*() const { return *_ptr; }
        pointer operator->() const { return _ptr; }
        SingleLinkList_Iterator<value_type>& operator++() {
            if (_ptr) _ptr = _ptr->next();
            else _ptr = nullptr;
            return *this;
        }
        SingleLinkList_Iterator<value_type>&  operator++(int) {
            if (_ptr) _ptr = _ptr->next();
            else _ptr = nullptr;
            return *this;
        }
        bool operator==(const SingleLinkList_Iterator<value_type>& obj) const { return _ptr == obj._ptr; }
        bool operator!=(const SingleLinkList_Iterator<value_type>& obj) const { return _ptr != obj._ptr; }

        pointer _ptr;
    };
public:
    using iterator = SingleLinkList_Iterator<T>;

public:
    explicit SingleLinkList() = default;
    SingleLinkList(uint32_t size, T value = T(), uint32_t maxLength = SINGLE_LINK_LIST_MAX_LENGTH);
    SingleLinkList(T* begin, T* end, uint32_t maxLength = SINGLE_LINK_LIST_MAX_LENGTH);
    SingleLinkList(SingleLinkList<T>&);
    SingleLinkList(const SingleLinkList<T>&&) = delete;
    ~SingleLinkList() {
        clear();
        _head = _tail = nullptr;
        delete _end;
    }

    void push(const T& value);
    void pop();
    [[nodiscard]] uint32_t length() const { return _length; }
    [[nodiscard]] uint32_t maxLength() const { return _maxLength; }
    void resize(uint32_t size) { _maxLength = size; }
    SingleLinkNode<T>& front() const { return *_head; }
    SingleLinkNode<T>& back() const { return *_tail; }
    iterator begin();
    iterator end();
    void clear();

    template<typename... Args>
    void run(function<void(T&, Args&...)> fun, Args&... args);

    SingleLinkList& operator=(const SingleLinkList&);
    SingleLinkList& operator=(const SingleLinkList&&) = delete;
private:
    SingleLinkNode<T>* _end = new SingleLinkNode<T>(T());
    SingleLinkNode<T>* _head{ _end };
    SingleLinkNode<T>* _tail{ _end };
    uint32_t _length{ 0 };
    uint32_t _maxLength = SINGLE_LINK_LIST_MAX_LENGTH;
};

NAME_SPACE_END()

#include "SingleLinkList.inl"

#endif // MAIN_SINGLELINKLIST_H
