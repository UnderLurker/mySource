//
// Created by 常笑男 on 2025/4/4.
//

#ifndef _M4A_TRACK_TIME_H
#define _M4A_TRACK_TIME_H

#include "m4a/m4a_common.h"

namespace myUtil {
// STTS
struct TimeToSampleBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> sampleCounts;
    std::unique_ptr<uint32_t[]> sampleDelta;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// CTTS
struct CompositionOffsetBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> sampleCount;
    std::unique_ptr<uint32_t[]> sampleOffsetU;
    std::unique_ptr<int32_t[]> sampleOffset;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// CSLG
struct CompositionToDecodeBox : public LeafFullBox {
    int64_t compositionToDTSShift;
    int64_t leastDecodeToDisplayDelta;
    int64_t greatestDecodeToDisplayDelta;
    int64_t compositionStartTime;
    int64_t compositionEndTime;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// STSS
struct SyncSampleBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> sampleNumber;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// STSH
struct ShadowSyncSampleBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> shadowedSampleNumber;
    std::unique_ptr<uint32_t[]> syncSampleNumber;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// SDTP
struct SampleDependencyTypeBox : public LeafFullBox {
    struct SampleDependencyTypeInfo {
        uint8_t isLeading           : 2;
        uint8_t sampleDependsOn     : 2;
        uint8_t sampleIsDependedOn  : 2;
        uint8_t sampleHasRedundancy : 2;
    };
    std::unique_ptr<SampleDependencyTypeInfo[]> infos;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// EDTS
struct EditBox : public Box {};
// ELST
struct EditListBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint64_t[]> segmentDuration64;
    std::unique_ptr<uint32_t[]> segmentDuration32;
    std::unique_ptr<int64_t[]> mediaTime64;
    std::unique_ptr<int32_t[]> mediaTime32;
    std::unique_ptr<int16_t[]> mediaRateInteger;
    std::unique_ptr<int16_t[]> mediaRateFraction;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_TRACK_TIME_H
