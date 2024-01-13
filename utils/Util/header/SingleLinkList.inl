#include "SingleLinkList.h"
#include <assert.h>

NAME_SPACE_START(myUtil)

template<typename T>
void SingleLinkNode<T>::insert(const T& value)
{
    auto tempNode = new SingleLinkNode<T>(value);
    tempNode->_next = _next;
    _next = tempNode;
}

template<typename T>
SingleLinkList<T>::SingleLinkList(uint32_t size, T value, uint32_t maxLength)
{
    assert(size <= maxLength);
    _maxLength = maxLength;
    _length = 0;
    for (uint32_t i = 0; i < size; i++) {
        this->push(T());
    }
}

template<typename T>
SingleLinkList<T>::SingleLinkList(T* begin, T* end, uint32_t maxLength)
{
    _maxLength = maxLength;
    _length = 0;
    auto l = begin;
    while(l != end) {
        push(*l);
        l++;
    }
}

template<typename T>
SingleLinkList<T>::SingleLinkList(SingleLinkList<T>& obj)
{
    _maxLength = obj._maxLength;
    _length = 0;
    auto _h = obj._head;
    for (uint32_t i = 0; i < obj._length; i++) {
        push(_h->value());
        _h = _h->next();
    }
}


template<typename T>
void SingleLinkList<T>::push(const T &value)
{
    if (_length >= _maxLength) return;
    if (_length == 0) {
        _head = new SingleLinkNode<T>(value, _end);
        _tail = _head;
        _end->setNext(_head);
        _length++;
        return;
    }
    auto tempNode = new SingleLinkNode<T>(value, _end);
    _tail->setNext(tempNode);
    _tail = tempNode;
    _length++;
}

template<typename T>
void SingleLinkList<T>::pop()
{
    if (_length <= 0 || !_head || !_tail) return;
    if (_length == 1) {
        delete _head;
        _head = _tail = _end;
        _length--;
        return;
    }
    auto nextPtr = _head->next();
    delete _head;
    _head = nextPtr;
    _end->setNext(_head);
    if (_head == _end) {
        _tail = _end;
    }
    _length--;
}

template<typename T>
typename SingleLinkList<T>::iterator SingleLinkList<T>::begin()
{
    return SingleLinkList_Iterator<T>{ ._ptr = _length == 0 ? nullptr : _head };
}

template<typename T>
typename SingleLinkList<T>::iterator SingleLinkList<T>::end()
{
    return SingleLinkList_Iterator<T>{ ._ptr = _length == 0 ? nullptr : _end };
}

template<typename T>
void SingleLinkList<T>::clear()
{
    for (uint32_t i = 0; i < _length; i++) {
        auto next = _head->next();
        delete _head;
        _head = next;
    }
    _head = _tail = _end;
    _length = 0;
}

template<typename T>
template<typename... Args>
void SingleLinkList<T>::run(function<void (T&, Args &...)> fun, Args&... args)
{
    fun(front(), args...);
}

template<typename T>
SingleLinkList<T>& SingleLinkList<T>::operator=(const SingleLinkList& obj)
{
    _maxLength = obj._maxLength;
    _length = 0;
    auto _h = obj._head;
    for (uint32_t i = 0; i < obj._length; i++) {
        push(_h->value());
        _h = _h->next();
    }
    return *this;
}

NAME_SPACE_END()