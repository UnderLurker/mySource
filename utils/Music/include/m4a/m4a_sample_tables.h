//
// Created by 常笑男 on 2025/4/4.
//

#ifndef _M4A_SAMPLE_TABLES_H
#define _M4A_SAMPLE_TABLES_H

#include "m4a/m4a_common.h"

namespace myUtil {
// STBL
struct SampleTableBox : public Box {};
// STSD
struct SampleDescriptionBox : public LeafFullBox {
    struct SampleEntry : public LeafBox {
        //    uint8_t reserved[6] = {0};
        uint16_t dataReferenceIndex;
    };
    uint32_t entryCount;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// STDP
struct DegradationPriorityBox : public LeafFullBox {
    std::unique_ptr<uint16_t[]> priorities;
    uint32_t len;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_SAMPLE_TABLES_H
