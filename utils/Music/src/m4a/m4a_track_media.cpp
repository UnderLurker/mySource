//
// Created by 常笑男 on 2025/4/4.
//
#include "m4a/m4a_track_media.h"

namespace myUtil {

M4AStatus MediaHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    auto tmpBody = body;
    if (_header.version == 1) {
        creationTime      = GetValue<uint64_t>(tmpBody);
        modificationTime  = GetValue<uint64_t>(tmpBody + 8);
        timeScale         = GetValue<uint32_t>(tmpBody + 16);
        duration          = GetValue<uint64_t>(tmpBody + 20);
        tmpBody          += 28;
    } else { // version == 0;
        creationTime      = GetValue<uint32_t>(tmpBody);
        modificationTime  = GetValue<uint32_t>(tmpBody + 4);
        timeScale         = GetValue<uint32_t>(tmpBody + 8);
        duration          = GetValue<uint32_t>(tmpBody + 12);
        tmpBody          += 16;
    }
    pad        = (tmpBody[0] & 0x80) == 0x80;
    language   = GetValue<uint16_t>(tmpBody) | 0x7F;
    preDefined = GetValue<uint16_t>(tmpBody + 2);
    return SUCCESS;
}

M4AStatus HandlerBox::OnProcessData(const uint8_t* body, size_t length) {
    preDefined  = GetValue<uint32_t>(body);
    handlerType = GetValue<uint32_t>(body + 4);
    for (size_t i = 0; i < sizeof(reserved) / sizeof(uint32_t); i++) {
        reserved[i] = GetValue<uint32_t>(body + 8 + 4 * i);
    }
    auto total = 8 + 4 * sizeof(reserved) / sizeof(uint32_t);
    std::string tmp(length - total, STRING_FILL);
    for (size_t i = 0; i < length - total; i++) {
        tmp[i] = static_cast<char>(*(body + total + i));
    }
    std::swap(name, tmp);
    return SUCCESS;
}

M4AStatus ExtendedLanguageBox::OnProcessData(const uint8_t* body, size_t length) {
    std::string tmp(length, STRING_FILL);
    for (size_t i = 0; i < length; i++) {
        tmp[i] = static_cast<char>(*(body + i));
    }
    std::swap(extendedLanguage, tmp);
    return SUCCESS;
}
} // namespace myUtil
