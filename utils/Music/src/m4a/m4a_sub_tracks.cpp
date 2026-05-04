//
// Created by 常笑男 on 2025/4/20.
//

#include "m4a/m4a_sub_tracks.h"

namespace myUtil {
M4AStatus SubTrackInformation::OnProcessData(const uint8_t* body, size_t length) {
    switchGroup    = GetValue<int16_t>(body);
    alternateGroup = GetValue<int16_t>(body + 2);
    subTrackID     = GetValue<uint32_t>(body + 4);
    int32_t count  = (length - 8) / sizeof(uint32_t);
    if (count <= 0) return SUCCESS;
    attributeList = std::make_unique<uint32_t[]>(count);
    for (int32_t i = 0; i < count; i++) {
        attributeList[i] = GetValue<uint32_t>(body + 8 + 4 * i);
    }
    return SUCCESS;
}
M4AStatus SubTrackSampleGroupBox::OnProcessData(const uint8_t* body, size_t length) {
    groupingType           = GetValue<uint32_t>(body);
    itemCount              = GetValue<uint16_t>(body + 4);
    groupDescriptionIndexs = std::make_unique<uint32_t[]>(itemCount);
    for (int32_t i = 0; i < itemCount; i++) {
        groupDescriptionIndexs[i] = GetValue<uint32_t>(body + 6 + 4 * i);
    }
    return SUCCESS;
}
} // namespace myUtil
