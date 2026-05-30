//
// Created by 常笑男 on 2026/5/30.
//

#ifndef _M4A_SEGMENTS_H
#define _M4A_SEGMENTS_H

#include "m4a/m4a_common.h"

namespace myUtil {
// STYP
struct SegmentTypeBox : public FileTypeBox {};
// SIDX
// startWithSAP SAP_type            reference_type  Meaning
// 0            0                   0 or 1          No information of SAPs is provided.
// 0            1 to 6, inclusive   0(media)        The subsegment contains (but may not start with) a SAP of the given
//                                                  SAP_type and the first SAP of the given SAP_type corresponds to
//                                                  SAP_delta_time.
// 0            1 to 6, inclusive   1(index)        All the referenced subsegments contain a SAP of at most the given
//                                                  SAP_type and none of these SAPs is of an unknown type.
// 1            0                   0(media)        The subsegment starts with a SAP of an unknown type.
// 1            0                   1(index)        All the referenced subsegments start with a SAP which may be of an
//                                                  unknown type.
// 1            1 to 6, inclusive   0(media)        The referenced subsegment starts with a SAP of the given SAP_type.
// 1            1 to 6, inclusive   1(index)        All the referenced subsegments start with a SAP of at most the given
//                                                  SAP_type and none of these SAPs is of an unknown type.
struct SegmentIndexBox : public LeafFullBox {
    struct ReferenceInfo {
        uint32_t type: 1;
        uint32_t size: 31;
    };
    struct SAPInfo {
        uint32_t startsWithSAP: 1;
        uint32_t type: 3;
        uint32_t deltaTime: 28;
    };
    uint32_t referenceId;
    uint32_t timescale;
    uint64_t earliestPresentationTime;
    uint64_t firstOffset;
    uint16_t reserved{0};
    uint16_t referenceCount;
    std::unique_ptr<ReferenceInfo[]> refInfos;
    std::unique_ptr<SAPInfo[]> sapInfos;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// SSIX
struct SubsegmentIndexBox : public LeafFullBox {
    struct RangeInfo {
        uint32_t level: 8;
        uint32_t size: 24;
    };
    struct RangeList {
        uint32_t count;
        std::unique_ptr<RangeInfo[]> infos;
    };
    uint32_t subsegmentCount;
    std::unique_ptr<RangeList[]> ranges;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// PRFT
struct ProducerReferenceTimeBox : public LeafFullBox {
    uint32_t referenceTrackId;
    uint64_t ntpTimeStamp;
    uint64_t mediaTime;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_SEGMENTS_H
