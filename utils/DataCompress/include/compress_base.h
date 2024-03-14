//
// Created by 常笑男 on 2024/2/10.
//

#ifndef _COMPRESS_BASE_H
#define _COMPRESS_BASE_H

#include "Util.h"

#include <cstdint>
#include <cassert>

namespace myUtil{

struct MatchResult {
    uint32_t pos { 0 };
    uint32_t length { 0 };
};

enum CompressStatus {
    SUCCESS = 0,
    ERROR_DECODE,
    ERROR_ENCODE,
    ERROR_NULLPTR,
    ERROR_UNKNOW
};

template<typename T>
class CompressBase {
public:
    virtual CompressStatus decode(const T* source, uint32_t sLength, T*& result, uint32_t& rLength) { return SUCCESS; }
    virtual CompressStatus encode(const T* source, uint32_t sLength, T*& result, uint32_t& rLength) { return SUCCESS; }
    virtual CompressStatus encode(const string& filePath, T*& result, uint32_t& rLength) { return SUCCESS; }

    /**
     * Find the longest target string in the source string
     * @param source source lp
     * @param sLen source length
     * @param target target lp
     * @param tLen target length
     * @return first is match length, second is place of begin in source
     */
    static MatchResult LCS(const T* source, uint32_t sLen, const T* target, uint32_t tLen)
    {
        assert(source != nullptr && target != nullptr);
        if (sLen == 0) return MatchResult { 0, 0 };
        MatchResult result;
        bool** dp = new bool*[tLen + 1];
        for (uint32_t i = 0; i < tLen + 1; i++) {
            dp[i] = new bool[sLen + 1];
            if (i == 0)
                memset(dp[i], 1, sLen + 1);
            else
                memset(dp[i], 0, sLen + 1);
        }
        for (uint32_t i = 1; i < tLen + 1; i++) {
            for (uint32_t j = 1; j < sLen + 1; j++) {
                if (source[j - 1] == target[i - 1] && dp[i - 1][j - 1]) {
                    dp[i][j] = true;
                    if (i >= result.length) {
                        result.length = i;
                        result.pos = j - i;
                    }
                }
            }
        }
        FREE_LP_2(dp, tLen + 1)
        return result;
    }
};

}

#endif // !_COMPRESS_BASE_H
