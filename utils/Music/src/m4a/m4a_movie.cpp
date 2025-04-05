//
// Created by 常笑男 on 2025/4/4.
//
#include "m4a/m4a_movie.h"

namespace myUtil {

M4AStatus MovieHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    if ((_header.version == 1 && length != 108) || (_header.version == 0 && length != 96)) return ERROR_FILE_FORMAT;
    auto pos = 0;
    if (_header.version == 1) {
        createTime       = GetValue<uint64_t>(body);
        modificationTime = GetValue<uint64_t>(body + 8);
        timeScale        = GetValue<uint32_t>(body + 16);
        duration         = GetValue<uint64_t>(body + 20);
        pos              = 28;
    } else { // version == 0
        createTime       = GetValue<uint32_t>(body);
        modificationTime = GetValue<uint32_t>(body + 4);
        timeScale        = GetValue<uint32_t>(body + 8);
        duration         = GetValue<uint32_t>(body + 12);
        pos              = 16;
    }
    rate        = GetValue<uint32_t>(body + pos);
    volume      = GetValue<int16_t>(body + pos + 4);
    nextTrackId = GetValue<uint32_t>(body + pos + 76);
    memcpy(reserved, body + pos + 6, 10);
    memcpy(matrix, body + pos + 16, 36);
    for (size_t i = 0; i < sizeof(matrix) / sizeof(int32_t); i++) {
        auto tmp  = body + pos + 16 + 4 * i;
        matrix[i] = GetValue<int32_t>(tmp);
    }
    for (size_t i = 0; i < sizeof(preDefined) / sizeof(int32_t); i++) {
        auto tmp      = body + pos + 52 + 4 * i;
        preDefined[i] = GetValue<int32_t>(tmp);
    }
    return SUCCESS;
}
} // namespace myUtil
