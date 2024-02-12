//
// Created by 常笑男 on 2024/2/11.
//

#ifndef _LZ77_H
#define _LZ77_H

#include "Util.h"
#include "compress_base.h"

#include <list>

#define SLIDE_WINDOW_LENGTH 8
#define FORWARD_BUFFER_LENGTH 4

NAME_SPACE_START(myUtil)

template<typename T = uint8_t>
struct DicCode {
    // 前向缓冲区中匹配开始位置距离滑动窗口中匹配开始位置的偏移量
    uint32_t _x;
    // 匹配的长度
    uint32_t _y;
    T* _valStart;
    T* _valEnd;
};

class LZ77 : public CompressBase<uint8_t> {
public:
    LZ77() = default;
    explicit LZ77(const list<DicCode<uint8_t>>& dic) : _dicQueue(dic), CompressBase<uint8_t>() {}
    ~LZ77() {
        reset();
        for(auto& item : _dicQueue) {
            if ((item._valEnd - item._valStart) > sizeof(uint8_t)) {
                delete[] item._valStart;
                item._valStart = item._valEnd = nullptr;
            } else {
                delete item._valStart;
                item._valStart = item._valEnd = nullptr;
            }
        }
    }
    CompressStatus decode(const uint8_t* source, uint32_t sLength, uint8_t* result, uint32_t& rLength) override;
    CompressStatus encode(const uint8_t* source, uint32_t sLength, uint8_t* result, uint32_t& rLength) override;

    void setDicQueue(const list<DicCode<uint8_t>>& dic) { _dicQueue = dic; }
    list<DicCode<uint8_t>> getDicQueue() const { return _dicQueue; }
private:
    void reset();
private:
    list<DicCode<uint8_t>> _dicQueue;
    uint32_t _windowStart { 0 };
    uint32_t _windowEnd { 0 };
    uint32_t _bufferStart { 0 };
    uint32_t _bufferEnd { FORWARD_BUFFER_LENGTH - 1 };
};

NAME_SPACE_END()

#endif // !_LZ77_H
