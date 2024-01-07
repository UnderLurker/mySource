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
    explicit SingleLinkNode(T value, SingleLinkNode<T>* next = nullptr, Deleter deleter = DefaultDeleter)
        : _data(value), _next(next), _deleter(deleter)
    {}
    ~SingleLinkNode()
    {
        _deleter(_data);
    }
    SingleLinkNode<T>* next() const { return _next; }
private:
    T _data{ T() };
    Deleter _deleter { DefaultDeleter };
    SingleLinkNode<T>* _next { nullptr };
};

template<typename T>
class SingleLinkList {
public:
    explicit SingleLinkList(uint32_t maxLength = SINGLE_LINK_LIST_MAX_LENGTH) : _maxLength(maxLength){}
    ~SingleLinkList() {
        for (uint32_t i = 0; i < _length; i++) {
            auto next = _head->next();
            delete _head;
            _head = next;
        }
    }

    void push(T& value);
    void pop();
    template<typename... Args>
    void run(function<void(Args&...)> fun, Args&... args)
    {
        fun(args...);
    }

private:
    SingleLinkNode<T>* _head{ nullptr };
    SingleLinkNode<T>* _tail{ nullptr };
    uint32_t _length{ 0 };
    uint32_t _maxLength = SINGLE_LINK_LIST_MAX_LENGTH;
};

NAME_SPACE_END()

#endif // MAIN_SINGLELINKLIST_H
