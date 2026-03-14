//
// Created by 常笑男 on 2026/3/14.
//
#include "m4a/m4a_support_for_protected_streams.h"

#include <cstring>

namespace myUtil {
bool OriginalFormatBox::ProcessFullBox(std::fstream& file) {
    auto* t = new uint8_t[sizeof(uint32_t)];
    file.read((char*)t, sizeof(uint32_t));
    dataFormat = GetValue<uint32_t>(t);
    delete[] t;
    return true;
}

M4AStatus SchemeTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    schemeType = GetValue<uint32_t>(body);
    schemeVersion = GetValue<uint32_t>(body + 4);
    if (_header.flags & 0x000001) {
        schemeUri = std::make_unique<uint8_t[]>(length - 8);
        memcpy(schemeUri.get(), body + 8, length - 8);
    }
    return SUCCESS;
}
} // namespace myUtil