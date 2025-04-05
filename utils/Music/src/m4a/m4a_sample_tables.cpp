//
// Created by 常笑男 on 2025/4/4.
//
#include "m4a/m4a_sample_tables.h"

namespace myUtil {

M4AStatus SampleDescriptionBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    for (size_t i = 0; i < entryCount; i++) {
        // process SampleEntry
    }
    return SUCCESS;
}

M4AStatus DegradationPriorityBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length < 2) return ERROR_FILE_FORMAT;
    priorities   = std::make_unique<uint16_t[]>(length / 2);
    uint32_t pos = 0;
    while (length >= 2) {
        priorities[pos++]  = GetValue<int16_t>(body + pos * 2);
        length            -= 2;
    }
    return SUCCESS;
}
} // namespace myUtil
