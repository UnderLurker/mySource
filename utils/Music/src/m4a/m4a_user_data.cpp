//
// Created by 常笑男 on 2025/4/12.
//
#include "m4a/m4a_user_data.h"

#include <cstring>

namespace myUtil {

M4AStatus CopyrightBox::OnProcessData(const uint8_t* body, size_t length) {
    language = GetValue<uint16_t>(body);
    if (length <= 2) return ERROR_FILE_FORMAT;
    notice.resize(length - 2);
    memcpy(const_cast<char*>(notice.c_str()), body + 2, length - 2);
    return SUCCESS;
}

M4AStatus TrackSelectionBox::OnProcessData(const uint8_t* body, size_t length) {
    switchGroup = GetValue<int32_t>(body);
    if (length <= 4) return ERROR_FILE_FORMAT;
    attributeList = std::make_unique<uint32_t[]>(length - 4);
    for (uint32_t i = 0; i < length - 4; i++) {
        attributeList[i] = GetValue<uint32_t>(body + 4 + 4 * i);
    }
    return SUCCESS;
}

M4AStatus KindBox::OnProcessData(const uint8_t* body, size_t length) {
    uint32_t endPos = 0;
    for (size_t i = 0; i < length; i++) {
        if (*(body + i) == '\0') {
            endPos = i;
            break;
        }
    }
    schemeURI = std::string((char*)body);
    value     = std::string((char*)(body + endPos + 1));
    return SUCCESS;
}
} // namespace myUtil
