//
// Created by 常笑男 on 2024/2/11.
//

#ifndef _LZ77_H
#define _LZ77_H

#include "Util.h"
#include "compress_base.h"

#include <list>

#define SLIDE_WINDOW_LENGTH 6
#define FORWARD_BUFFER_LENGTH 3
//#define LZ77_DEBUG

NAME_SPACE_START(myUtil)

template<typename T = uint8_t>
struct DicCode {
    // 前向缓冲区中匹配开始位置距离滑动窗口中匹配开始位置的偏移量
    uint32_t _x;
    // 匹配的长度
    uint32_t _y;
    T* _valStart;
};

template<typename T = uint8_t>
class LZ77 : public CompressBase<T> {
public:
    LZ77() = default;
    LZ77(const LZ77&) = delete;
    LZ77(const LZ77&&) = delete;
    explicit LZ77(const list<DicCode<T>>& dic);
    LZ77& operator=(const LZ77&) = delete;
    ~LZ77();
    /**
     * decode source code with LZ77 to result
     * @param source unused
     * @param sLength unused
     * @param result
     * @param rLength
     * @return status of decode
     */
    CompressStatus decode(const T* source, uint32_t sLength, T*& result, uint32_t& rLength) override;
    /**
     * encode source code with LZ77 to result,
     * @param source input
     * @param sLength length of source
     * @param result unused
     * @param rLength unused
     * @return status of encode
     */
    CompressStatus encode(const T* source, uint32_t sLength, T*& result, uint32_t& rLength) override;
    CompressStatus encode(const string& filePath, T*& result, uint32_t& rLength) override;

    void setDicQueue(const list<DicCode<T>>& dic) { _dicQueue = dic; }
    list<DicCode<T>> getDicQueue() const { return _dicQueue; }
private:
    void reset();
    uint32_t getDecodeLength() const;
private:
    list<DicCode<T>> _dicQueue;
    uint32_t _windowStart { 0 };
    uint32_t _windowEnd { 0 };
    uint32_t _bufferStart { 0 };
    uint32_t _bufferEnd { FORWARD_BUFFER_LENGTH - 1 };
};

NAME_SPACE_END()

#include "LZ77.inl"
#endif // !_LZ77_H
