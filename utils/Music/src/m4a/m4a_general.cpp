//
// Created by 常笑男 on 2025/4/4.
//
#include "m4a/m4a_general.h"

namespace myUtil {

M4AStatus MediaDataBox::OnProcessData(const uint8_t* body, size_t length) {
    data = std::make_unique<uint8_t[]>(length);
    memcpy(data.get(), body, length);
    len = length;
    return SUCCESS;
}

M4AStatus ProgressiveDownloadInfoBox::OnProcessData(const uint8_t* body, size_t length) {
    size_t size = sizeof(PdinInfo);
    if (length % size != 0) return ERROR_FILE_FORMAT;
    size_t l = 0 + FULL_BOX_LEN, r = 4 + FULL_BOX_LEN;
    while (length) {
        infos.push_back({GetValue<uint32_t>(body + l), GetValue<uint32_t>(body + l + 4)});
        length -= size;
        l      += size;
        r      += size;
    }
    return SUCCESS;
}
} // namespace myUtil
