//
// Created by 常笑男 on 2025/4/4.
//
#include "m4a/m4a_track_time.h"

#include "m4a/m4a_track_data_layout.h"

namespace myUtil {

M4AStatus TimeToSampleBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (entryCount != (length - 4) / 8) return ERROR_FILE_FORMAT;
    sampleCounts = std::make_unique<uint32_t[]>(entryCount);
    sampleDelta  = std::make_unique<uint32_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        sampleCounts[i] = GetValue<uint32_t>(body + 4 + 8 * i);
        sampleDelta[i]  = GetValue<uint32_t>(body + 8 + 8 * i);
    }
    return SUCCESS;
}

M4AStatus CompositionOffsetBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (entryCount != (length - 4) / 8) return ERROR_FILE_FORMAT;
    sampleCount = std::make_unique<uint32_t[]>(entryCount);
    if (_header.version == 0) {
        sampleOffsetU = std::make_unique<uint32_t[]>(entryCount);
        for (size_t i = 0; i < entryCount; i++) {
            sampleCount[i]   = GetValue<uint32_t>(body + 4 + 8 * i);
            sampleOffsetU[i] = GetValue<uint32_t>(body + 8 + 8 * i);
        }
    } else { // version == 1
        sampleOffset = std::make_unique<int32_t[]>(entryCount);
        for (size_t i = 0; i < entryCount; i++) {
            sampleCount[i]  = GetValue<uint32_t>(body + 4 + 8 * i);
            sampleOffset[i] = GetValue<int32_t>(body + 8 + 8 * i);
        }
    }
    return SUCCESS;
}

M4AStatus CompositionToDecodeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (_header.version == 0) {
        MYASSERT(length == 20);
        compositionToDTSShift        = GetValue<int32_t>(body);
        leastDecodeToDisplayDelta    = GetValue<int32_t>(body + 4);
        greatestDecodeToDisplayDelta = GetValue<int32_t>(body + 8);
        compositionStartTime         = GetValue<int32_t>(body + 12);
        compositionEndTime           = GetValue<int32_t>(body + 16);
    } else { // version == 1
        MYASSERT(length == 40);
        compositionToDTSShift        = GetValue<int64_t>(body);
        leastDecodeToDisplayDelta    = GetValue<int64_t>(body + 8);
        greatestDecodeToDisplayDelta = GetValue<int64_t>(body + 16);
        compositionStartTime         = GetValue<int64_t>(body + 24);
        compositionEndTime           = GetValue<int64_t>(body + 32);
    }
    return SUCCESS;
}

M4AStatus SyncSampleBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (length != (entryCount * 4 + 4)) return ERROR_FILE_FORMAT;
    sampleNumber = std::make_unique<uint32_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        sampleNumber[i] = GetValue<uint32_t>(body + 4 + 4 * i);
    }
    return SUCCESS;
}

M4AStatus ShadowSyncSampleBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (length != (entryCount * 8 + 4)) return ERROR_FILE_FORMAT;
    shadowedSampleNumber = std::make_unique<uint32_t[]>(entryCount);
    syncSampleNumber     = std::make_unique<uint32_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        shadowedSampleNumber[i] = GetValue<uint32_t>(body + 4 + 8 * i);
        syncSampleNumber[i]     = GetValue<uint32_t>(body + 8 + 8 * i);
    }
    return SUCCESS;
}

M4AStatus SampleDependencyTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    bool flag1 = ParentCount(BoxType::STSZ) == 0;
    bool flag2 = ParentCount(BoxType::STZ2) == 0;
    if (flag1 && flag2) return ERROR_FILE_FORMAT;
    auto boxList = flag1 ? GetBoxList(BoxType::STZ2) : GetBoxList(BoxType::STSZ);
    MYASSERT(!boxList.empty())
    uint32_t entryCount = 0;
    if (!flag1) {
        auto stsz  = dynamic_pointer_cast<SampleSizeBox>(boxList.front());
        entryCount = stsz->sampleCount;
    } else {
        auto stz2  = dynamic_pointer_cast<CompactSampleSizeBox>(boxList.front());
        entryCount = stz2->sampleCount;
    }
    if (entryCount != length) return ERROR_FILE_FORMAT;
    infos = std::make_unique<SampleDependencyTypeInfo[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        auto tmp                     = GetValue<uint8_t>(body + i);
        infos[i].isLeading           = (tmp & 0b11000000) >> 6;
        infos[i].sampleDependsOn     = (tmp & 0b00110000) >> 4;
        infos[i].sampleIsDependedOn  = (tmp & 0b00001100) >> 2;
        infos[i].sampleHasRedundancy = tmp & 0b00000011;
    }
    return SUCCESS;
}

M4AStatus EditListBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (_header.version == 1) {
        segmentDuration64 = std::make_unique<uint64_t[]>(entryCount);
        mediaTime64       = std::make_unique<int64_t[]>(entryCount);
    } else { // version == 0
        segmentDuration32 = std::make_unique<uint32_t[]>(entryCount);
        mediaTime32       = std::make_unique<int32_t[]>(entryCount);
    }
    mediaRateInteger  = std::make_unique<int16_t[]>(entryCount);
    mediaRateFraction = std::make_unique<int16_t[]>(entryCount);
    auto tmpBody      = body + 4;
    for (size_t i = 0; i < entryCount; i++) {
        if (_header.version == 1) {
            segmentDuration64[i] = GetValue<uint64_t>(tmpBody);
            mediaTime64[i]       = GetValue<int64_t>(tmpBody + 8);
            tmpBody              = tmpBody + 16;
        } else { // version == 0
            segmentDuration32[i] = GetValue<uint32_t>(tmpBody);
            mediaTime32[i]       = GetValue<int32_t>(tmpBody + 4);
            tmpBody              = tmpBody + 8;
        }
        mediaRateInteger[i]  = GetValue<int16_t>(tmpBody);
        mediaRateFraction[i] = GetValue<int16_t>(tmpBody + 2);
        tmpBody              = tmpBody + 4;
    }
    return SUCCESS;
}
} // namespace myUtil
