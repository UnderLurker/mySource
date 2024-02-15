#include "heap.h"

NAME_SPACE_START(myUtil)

template<typename T, typename Compare>
heap<T, Compare>::heap(const heap& obj)
{
    _curLength = obj._curLength;
    _maxLength = obj._maxLength;
    _values = new T[obj._maxLength] { T() };
    memcpy_s(_values, sizeof(T) * _curLength, obj._values, sizeof(T) * obj._curLength);
}

template<typename T, typename Compare>
heap<T, Compare>::heap(uint32_t size, T val)
{
    _curLength = size;
    _maxLength = size;
    _values = new T[size] { val };
}

template<typename T, typename Compare>
heap<T, Compare>::heap(T* begin, T* end, uint32_t length)
{
    assert(begin != nullptr && end != nullptr);
    uint32_t len = length == -1 ? (end - begin) : length;
    _curLength = len;
    _maxLength = len;
    _values = new T[len] { T() };
    for (uint32_t i = 0; i < len;i++) {
        _values[i] = *(begin + i);
    }
    balanceAll();
}

template<typename T, typename Compare>
heap<T, Compare>& heap<T, Compare>::operator=(const heap& obj)
{
    _curLength = obj._curLength;
    _maxLength = obj._maxLength;
    _values = new T[obj._maxLength] { T() };
    memcpy_s(_values, sizeof(T) * _curLength, obj._values, sizeof(T) * obj._curLength);
    return *this;
}

template<typename T, typename Compare>
void heap<T, Compare>::resize(uint32_t size)
{
    auto* tmp = _values;
    _maxLength = size;
    _values = new T[_maxLength] { T() };;
    memcpy_s(_values, sizeof(T) * _curLength, tmp, sizeof(T) * _curLength);
    delete[] tmp;
}

template<typename T, typename Compare>
bool heap<T, Compare>::empty() const
{
    return _curLength == 0;
}

template<typename T, typename Compare>
T heap<T, Compare>::front() const
{
    assert(_curLength > 0);
    return *_values;
}

template<typename T, typename Compare>
T heap<T, Compare>::back() const
{
    assert(_curLength > 0);
    return *(_values + _curLength - 1);
}

template<typename T, typename Compare>
void heap<T, Compare>::push_back(const T& val)
{
    if (_curLength == _maxLength) {
        auto* tmp = _values;
        _maxLength = _maxLength == 0 ? 1 : AMPLIFY_NUM * _maxLength;
        _values = new T[_maxLength] { T() };
        memcpy_s(_values, sizeof(T) * _curLength, tmp, sizeof(T) * _curLength);
        delete[] tmp;
    }
    _values[_curLength] = val;
    _curLength++;
    balanceAll();
}

template<typename T, typename Compare>
void heap<T, Compare>::pop_back()
{
    if (_curLength <= 0) return;
    _curLength--;
}

template<typename T, typename Compare>
void heap<T, Compare>::erase(uint32_t pos)
{
    assert(pos >= 0);
    if (pos >= _curLength) return;
    _values[pos] = _values[_curLength - 1];
    _curLength--;
    balance(pos);
}

template<typename T, typename Compare>
void heap<T, Compare>::erase(iterator iter)
{
    assert((iter._ptr - _values) >= 0);
    erase(iter._ptr - _values);
}

template<typename T, typename Compare>
void heap<T, Compare>::erase(reverse_iterator iter)
{
    assert((iter._ptr - _values) <= _curLength);
    erase(iter._ptr - _values);
}

template<typename T, typename Compare>
typename heap<T, Compare>::iterator heap<T, Compare>::begin() const
{
    return Iterator<T>{ ._ptr = _values };
}

template<typename T, typename Compare>
typename heap<T, Compare>::iterator heap<T, Compare>::end() const
{
    return Iterator<T>{ ._ptr = (_values + _curLength) };
}

template<typename T, typename Compare>
typename heap<T, Compare>::reverse_iterator heap<T, Compare>::rbegin() const
{
    return ReverseIterator<T>{ ._ptr = (_values + _curLength - 1) };
}

template<typename T, typename Compare>
typename heap<T, Compare>::reverse_iterator heap<T, Compare>::rend() const
{
    return ReverseIterator<T>{ ._ptr = (_values  - 1) };
}

//template<typename T, typename Compare>
//const Iterator<T> heap<T, Compare>::cbegin() const
//{
//    return Iterator<T>{ ._ptr = _values };
//}
//
//template<typename T, typename Compare>
//const Iterator<T> heap<T, Compare>::cend() const
//{
//    return Iterator<T>{ ._ptr = (_values + _curLength) };
//}
//
//template<typename T, typename Compare>
//typename heap<T, Compare>::const_reverse_iterator heap<T, Compare>::crbegin() const
//{
//    return Iterator<T>{ ._ptr = (_values + _curLength - 1) };
//}
//
//template<typename T, typename Compare>
//typename heap<T, Compare>::const_reverse_iterator heap<T, Compare>::crend() const
//{
//    return Iterator<T>{ ._ptr = (_values  - 1) };
//}

template<typename T, typename Compare>
void heap<T, Compare>::swap(T& lPos, T& rPos)
{
    T tmp = lPos;
    lPos = rPos;
    rPos = tmp;
}

template<typename T, typename Compare>
template<typename... Args>
void heap<T, Compare>::emplace_back(Args&&... args)
{
    push_back(T(args...));
}

#ifdef HEAP_PRINT_DEBUG
template<typename T, typename Compare>
void heap<T, Compare>::print(const char* info)
{
    if (info) {
        cout << info << ":";
    }
    for (uint32_t i = 0; i < _curLength; i++) {
        cout << _values[i] << " ";
    }
    cout << endl;
}
#endif

template<typename T, typename Compare>
void heap<T, Compare>::balance(uint32_t pos)
{
#ifdef HEAP_DEBUG
    cout << pos << " ";
#endif
    T tmp;
    Compare compare;
    uint32_t tmpPos = pos + 1;
    if (_curLength <= 1 || tmpPos > _curLength / 2) return;
    assert(tmpPos > 0);
    bool rFlag = (tmpPos * 2 < _curLength);
    T &cur = _values[tmpPos - 1];
    T &lChild = _values[tmpPos * 2 - 1];
    T &rChild = rFlag ? _values[tmpPos * 2] : tmp;
    if (compare(cur, lChild) && (!rFlag || compare(cur, rChild))) return;
    else if ((!rFlag && compare(lChild, cur)) || compare(lChild, rChild)) {
        swap(cur, lChild);
        balance(tmpPos * 2 - 1);
    } else {
        swap(cur, rChild);
        balance(tmpPos * 2);
    }
}

template<typename T, typename Compare>
void heap<T, Compare>::balanceAll()
{
    for (int32_t i = _curLength / 2 - 1; i >= 0; i--) {
        balance(i);
#ifdef HEAP_DEBUG
        cout<<endl;
#endif
    }
}

NAME_SPACE_END()