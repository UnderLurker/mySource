//
// Created by 常笑男 on 2024/2/11.
//
#include "LZ77.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

CompressStatus LZ77::decode(const uint8_t* source, uint32_t sLength, uint8_t* result, uint32_t& rLength)
{
    assert(!_dicQueue.empty());
    return ERROR_UNKNOW;
}

CompressStatus LZ77::encode(const uint8_t* source, uint32_t sLength, uint8_t* result, uint32_t& rLength)
{
    assert(_dicQueue.empty());
    try{
        while(_windowEnd < sLength) {
            uint32_t slideLength = 0;
            auto findRes = LCS(source + _windowStart, _windowEnd - _windowStart,
                source + _bufferStart, _bufferEnd - _bufferStart);
            cout<<"pos:"<<findRes.pos<<" length:"<<findRes.length<<endl;
            if (findRes.length == 0) {
                auto* res = new uint8_t;
                *res = *(source + _bufferStart);
                _dicQueue.push_back(DicCode<uint8_t> { 0, 0, res, res + 1 });
                slideLength = 1;
            } else {
                auto* res = new uint8_t[findRes.length + 1];
                memcpy_s(res, findRes.length, source + _windowStart + findRes.pos, findRes.length);
                res[findRes.length] = '\0';
                _dicQueue.push_back(DicCode<uint8_t> {
                    _bufferStart - _windowStart - findRes.pos,
                    findRes.length,
                    res,
                    &res[findRes.length]
                });
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
    }catch(...){
        return ERROR_UNKNOW;
    }
    return SUCCESS;
}

void LZ77::reset()
{
    _windowStart = 0;
    _windowEnd = 0;
    _bufferStart = 0;
    _bufferEnd = FORWARD_BUFFER_LENGTH - 1;
}

NAME_SPACE_END()