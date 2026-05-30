//
// Created by 常笑男 on 2026/5/30.
//
#include "m4a/m4a_segments.h"

namespace myUtil {

M4AStatus SegmentIndexBox::OnProcessData(const uint8_t* body, size_t length) {
    uint32_t pos = 0;
    ReadData(body, referenceId, pos);
    ReadData(body, timescale, pos);
    if (_header.version == 0) {
        earliestPresentationTime  = GetValue<uint32_t>(body + pos);
        firstOffset               = GetValue<uint32_t>(body + pos + 4);
        pos                      += 8;
    } else {
        ReadData(body, earliestPresentationTime, pos);
        ReadData(body, firstOffset, pos);
    }
    ReadData(body, reserved, pos);
    ReadData(body, referenceCount, pos);
    refInfos = std::make_unique<ReferenceInfo[]>(referenceCount);
    sapInfos = std::make_unique<SAPInfo[]>(referenceCount);
    for (uint32_t i = 0; i < referenceCount; i++) {
        uint32_t info = 0;
        ReadData(body, info, pos);
        refInfos[i].type = info >> 31;
        refInfos[i].size = info & 0x7FFFFFFF;
        ReadData(body, info, pos);
        sapInfos[i].startsWithSAP = info >> 31;
        sapInfos[i].type          = (info >> 28) & 0x7;
        sapInfos[i].deltaTime     = info & 0xFFFFFFF;
    }
    return SUCCESS;
}

M4AStatus SubsegmentIndexBox::OnProcessData(const uint8_t* body, size_t length) {
    uint32_t pos = 0;
    ReadData(body, subsegmentCount, pos);
    ranges = std::make_unique<RangeList[]>(subsegmentCount);
    for (uint32_t i = 0; i < subsegmentCount; i++) {
        ReadData(body, ranges[i].count, pos);
        ranges[i].infos = std::make_unique<RangeInfo[]>(ranges[i].count);
        for (uint32_t j = 0; j < ranges[i].count; j++) {
            uint32_t info = 0;
            ReadData(body, info, pos);
            ranges[i].infos[j].level = info >> 24;
            ranges[i].infos[j].size  = info & 0xFFFFFF;
        }
    }
    return SUCCESS;
}

M4AStatus ProducerReferenceTimeBox::OnProcessData(const uint8_t* body, size_t length) {
    uint32_t pos = 0;
    ReadData(body, referenceTrackId, pos);
    ReadData(body, ntpTimeStamp, pos);
    if (_header.version == 0) {
        mediaTime = GetValue<uint32_t>(body + pos);
    } else {
        ReadData(body, mediaTime, pos);
    }
    return SUCCESS;
}
} // namespace myUtil
