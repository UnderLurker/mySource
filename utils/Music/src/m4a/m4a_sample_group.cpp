//
// Created by 常笑男 on 2025/4/9.
//
#include "m4a/m4a_sample_group.h"

namespace myUtil {

M4AStatus SampleToGroupBox::OnProcessData(const uint8_t* body, size_t length) {
    groupingType = GetValue<uint32_t>(body);
    uint32_t pos = 4;
    if (_header.version == 1) {
        groupingTypeParameter  = GetValue<uint32_t>(body + pos);
        pos                   += 4;
    }
    count  = GetValue<uint32_t>(body + pos);
    pos   += 4;
    if (count == 0) return SUCCESS;
    sampleCounts            = std::make_unique<uint32_t[]>(count);
    groupDescriptionIndexes = std::make_unique<uint32_t[]>(count);
    for (uint32_t i = 0; i < count; i++) {
        sampleCounts[i]            = GetValue<uint32_t>(body + pos + 8 * i);
        groupDescriptionIndexes[i] = GetValue<uint32_t>(body + pos + 8 * i + 4);
    }
    return SUCCESS;
}
} // namespace myUtil
