//
// Created by 常笑男 on 2025/4/4.
//
#include "m4a/m4a_track.h"

namespace myUtil {

M4AStatus TrackHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    const uint8_t* tmpBody = body;
    if (_header.version == 1) {
        creationTime     = GetValue<uint64_t>(tmpBody);
        modificationTime = GetValue<uint64_t>(tmpBody + 8);
        trackId          = GetValue<uint32_t>(tmpBody + 16);
        reserved         = GetValue<uint32_t>(tmpBody + 20);
        duration         = GetValue<uint64_t>(tmpBody + 24);
        tmpBody          = tmpBody + 32;
    } else { // version == 0
        creationTime     = GetValue<uint32_t>(tmpBody);
        modificationTime = GetValue<uint32_t>(tmpBody + 4);
        trackId          = GetValue<uint32_t>(tmpBody + 8);
        reserved         = GetValue<uint32_t>(tmpBody + 12);
        duration         = GetValue<uint32_t>(tmpBody + 16);
        tmpBody          = tmpBody + 20;
    }
    tmpBody        = tmpBody + 8;
    layer          = GetValue<int16_t>(tmpBody);
    alternateGroup = GetValue<int16_t>(tmpBody + 2);
    volume         = GetValue<int16_t>(tmpBody + 4);
    width          = GetValue<uint32_t>(tmpBody + 44);
    height         = GetValue<uint32_t>(tmpBody + 48);
    for (size_t i = 0; i < sizeof(matrix) / sizeof(int32_t); i++) {
        auto tmp  = tmpBody + 8 + 4 * i;
        matrix[i] = GetValue<int32_t>(tmp);
    }
    return SUCCESS;
}

M4AStatus TrackReferenceTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length % 4 != 0) return ERROR_FILE_FORMAT;
    len      = length / 4;
    trackIDs = std::make_unique<uint32_t[]>(len);
    for (size_t i = 0; i < length; i += 4) {
        trackIDs[i] = GetValue<uint32_t>(body + i);
    }
    return SUCCESS;
}

M4AStatus TrackGroupTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length != sizeof(trackGroupId)) return ERROR_FILE_FORMAT;
    trackGroupId = GetValue<uint32_t>(body);
    return SUCCESS;
}
} // namespace myUtil
