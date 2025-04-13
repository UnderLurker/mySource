//
// Created by 常笑男 on 2025/4/9.
//

#ifndef _M4A_SAMPLE_GROUP_H
#define _M4A_SAMPLE_GROUP_H

#include "m4a/m4a_common.h"

namespace myUtil {
// SBGP
struct SampleToGroupBox : public LeafFullBox {
    uint32_t groupingType;
    uint32_t groupingTypeParameter;
    uint32_t count;
    std::unique_ptr<uint32_t[]> sampleCounts;
    std::unique_ptr<uint32_t[]> groupDescriptionIndexes;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// SGPD
struct SampleGroupDescriptionBox : public LeafFullBox {
    // unknown
};
} // namespace myUtil

#endif // _M4A_SAMPLE_GROUP_H
