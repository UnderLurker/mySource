//
// Created by 常笑男 on 2024/2/11.
//
#include "LZ77.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

template<typename T>
LZ77<T>::LZ77(const list<DicCode<T>>& dic) : CompressBase<T>() {
    for(const auto& item : dic) {
        DicCode<T> tmp { item._x, item._y, nullptr };
        if (item._x == 0 && item._y == 0 && item._valStart) {
            tmp._valStart = new T{ *item._valStart };
        }
        _dicQueue.emplace_back(tmp);
    }
}

template<typename T>
LZ77<T>::~LZ77() {
    reset();
    for(auto& item : _dicQueue) {
        if (item._y != 0) {
            delete[] item._valStart;
            item._valStart = nullptr;
        } else {
            delete item._valStart;
            item._valStart = nullptr;
        }
    }
}

template<typename T>
CompressStatus LZ77<T>::decode(const T* source, uint32_t sLength, T*& result, uint32_t& rLength)
{
    assert(!_dicQueue.empty() && result == nullptr);
    uint32_t curPos = 0;
    try {
        rLength = getDecodeLength();
        result = new T[rLength];
        memset(result, 0, rLength);
        for (const auto& item : _dicQueue) {
            if (item._x == 0 && item._y == 0) {
                result[curPos++] = *item._valStart;
            } else {
                memcpy_s(result + curPos, item._y, result + curPos - item._x, item._y);
                curPos += item._y;
            }
        }
    } catch (...) {
        return ERROR_UNKNOW;
    }
    return SUCCESS;
}

template<typename T>
CompressStatus LZ77<T>::encode(const T* source, uint32_t sLength, T*& result, uint32_t& rLength)
{
    assert(_dicQueue.empty());
    CHECK_NULL_RETURN(source, ERROR_NULLPTR)
    try{
        while(_windowEnd < sLength) {
            uint32_t slideLength = 0;
            auto findRes = CompressBase<T>::LCS(source + _windowStart, _windowEnd - _windowStart,
                source + _bufferStart, _bufferEnd - _bufferStart);
            if (findRes.length == 0) {
                auto* res = new T;
                *res = *(source + _bufferStart);
                _dicQueue.push_back(DicCode<T> { 0, 0, res });
                slideLength = 1;
            } else {
#ifdef LZ77_DEBUG
                auto* res = new T[findRes.length];
                memcpy_s(res, findRes.length, source + _windowStart + findRes.pos, findRes.length);
                _dicQueue.push_back(DicCode<T> {
                    _bufferStart - _windowStart - findRes.pos,
                    findRes.length,
                    res
                });
#else
                _dicQueue.push_back(DicCode<T> {
                    _bufferStart - _windowStart - findRes.pos,
                    findRes.length,
                    nullptr
                });
#endif
                slideLength = findRes.length;
            }
            // 窗口滑动
            if (_windowEnd - _windowStart == SLIDE_WINDOW_LENGTH)
                _windowStart += slideLength;
            _windowEnd += slideLength;
            _bufferStart += slideLength;
            _bufferEnd += slideLength;
            if (_bufferEnd > sLength)
                _bufferEnd = sLength;
        }
        reset();
    } catch (...) {
        return ERROR_UNKNOW;
    }
    return SUCCESS;
}

template<typename T>
CompressStatus LZ77<T>::encode(const string& filePath, T*& result, uint32_t& rLength)
{
    try {
        fstream file(filePath, ios::in | ios::binary);
        // something
    } catch (...) {
        return ERROR_UNKNOW;
    }
    return SUCCESS;
}

template<typename T>
void LZ77<T>::reset()
{
    _windowStart = 0;
    _windowEnd = 0;
    _bufferStart = 0;
    _bufferEnd = FORWARD_BUFFER_LENGTH - 1;
}

template<typename T>
uint32_t LZ77<T>::getDecodeLength() const
{
    uint32_t result = 0;
    for (const auto& item : _dicQueue) {
        if (item._y == 0)
            result += 1;
        else
            result += item._y;
    }
    return result;
}

NAME_SPACE_END()