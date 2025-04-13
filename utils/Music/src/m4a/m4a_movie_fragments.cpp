//
// Created by 常笑男 on 2025/4/5.
//
#include "m4a/m4a_movie_fragments.h"

namespace myUtil {
namespace {

bool CheckFlag(uint32_t flags, uint32_t check) { return (flags & check) == check; };
} // namespace

M4AStatus MovieExtendsHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    if (_header.version == 1) {
        fragmentDuration = GetValue<uint64_t>(body);
    } else { // version == 0
        fragmentDuration = GetValue<uint32_t>(body);
    }
    return SUCCESS;
}

M4AStatus TrackExtendsBox::OnProcessData(const uint8_t* body, size_t length) {
    trackId                   = GetValue<uint32_t>(body);
    descriptionIndex          = GetValue<uint32_t>(body + 4);
    duration                  = GetValue<uint32_t>(body + 8);
    size                      = GetValue<uint32_t>(body + 12);
    flags.reserved            = 0;
    flags.isLeading           = (GetValue<uint8_t>(body + 16) >> 2) & 0x03;
    flags.dependsOn           = GetValue<uint8_t>(body + 16) & 0x03;
    flags.isDependedOn        = (GetValue<uint8_t>(body + 17) >> 6) & 0x03;
    flags.hasRedundancy       = (GetValue<uint8_t>(body + 17) >> 4) & 0x03;
    flags.paddingValue        = (GetValue<uint8_t>(body + 17) >> 1) & 0x07;
    flags.isNonSyncSample     = GetValue<uint8_t>(body + 17) & 0x01;
    flags.degradationPriority = GetValue<uint16_t>(body + 18);
    return SUCCESS;
}

M4AStatus MovieFragmentHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    sequenceNumber = GetValue<uint32_t>(body);
    return SUCCESS;
}

M4AStatus TrackFragmentHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    trackId      = GetValue<uint32_t>(body);
    uint32_t pos = 4;
    if (CheckFlag(_header.flags, 0x000001)) { ReadData(body, baseDataOffset, pos); }
    if (CheckFlag(_header.flags, 0x000002)) { ReadData(body, sampleDescriptionIndex, pos); }
    if (CheckFlag(_header.flags, 0x000008)) { ReadData(body, defaultSampleDuration, pos); }
    if (CheckFlag(_header.flags, 0x000010)) { ReadData(body, defaultSampleSize, pos); }
    if (CheckFlag(_header.flags, 0x000020)) { ReadData(body, defaultSampleFlags, pos); }
    if (CheckFlag(_header.flags, 0x010000)) { defaultSampleDuration.reset(); }
    if (!baseDataOffset && CheckFlag(_header.flags, 0x020000)) {
        // ...
    }
    return SUCCESS;
}

M4AStatus TrackRunBox::OnProcessData(const uint8_t* body, size_t length) {
    count = GetValue<uint32_t>(body);
    if (count == 0) return SUCCESS;
    uint32_t pos = 4;
    if (CheckFlag(_header.flags, 0x000001)) ReadData(body, dataOffset, pos);
    if (CheckFlag(_header.flags, 0x000004)) ReadData(body, firstSampleFlags, pos);
    infos = std::make_unique<TrackRunInfo[]>(count);
    for (uint32_t i = 0; i < count; i++) {
        if (CheckFlag(_header.flags, 0x000100)) ReadData(body, infos[i].duration, pos);
        if (CheckFlag(_header.flags, 0x000200)) ReadData(body, infos[i].size, pos);
        if (CheckFlag(_header.flags, 0x000400)) ReadData(body, infos[i].flags, pos);
        if (_header.version == 0 && CheckFlag(_header.flags, 0x000800))
            ReadData(body, infos[i].compositionTimeOffset, pos);
        else if (_header.version != 0 && CheckFlag(_header.flags, 0x000800))
            ReadData(body, infos[i].uCompositionTimeOffset, pos);
    }
    return SUCCESS;
}

M4AStatus TrackFragmentRandomAccessBox::OnProcessData(const uint8_t* body, size_t length) {
    trackId              = GetValue<uint32_t>(body);
    auto lengthInfo      = GetValue<uint8_t>(body + 7);
    lengthSize.reserved  = 0;
    lengthSize.trafNum   = (lengthInfo >> 4) & 0x03;
    lengthSize.trunNum   = (lengthInfo >> 2) & 0x03;
    lengthSize.sampleNum = lengthInfo & 0x03;
    entryCount           = GetValue<uint32_t>(body + 8);
    if (entryCount == 0) return SUCCESS;
    entries       = std::make_unique<LengthSizeEntry[]>(entryCount);
    uint32_t pos  = 12;
    auto getValue = [](const uint8_t* data, uint32_t size) {
        uint32_t result = 0;
        for (uint32_t i = 0; i < size; i++) {
            result = (result << 8) | data[i];
        }
        return result;
    };
    for (uint32_t i = 0; i < entryCount; i++) {
        if (_header.version == 1) {
            entries[i].time64        = GetValue<uint64_t>(body + pos);
            entries[i].moofOffset64  = GetValue<uint64_t>(body + pos + 8);
            pos                     += 16;
        } else {
            entries[i].time32        = GetValue<uint64_t>(body + pos);
            entries[i].moofOffset32  = GetValue<uint64_t>(body + pos + 4);
            pos                     += 8;
        }
        entries[i].trafNumber = getValue(body + pos, lengthSize.trafNum + 1);
        entries[i].trunNumber = getValue(body + pos + lengthSize.trafNum + 1, lengthSize.trunNum + 1);
        entries[i].sampleNumber =
            getValue(body + pos + lengthSize.trunNum + lengthSize.trafNum + 2, lengthSize.sampleNum + 1);
        pos += lengthSize.trunNum + 3 + lengthSize.trafNum + lengthSize.sampleNum;
    }
    return SUCCESS;
}

M4AStatus MovieFragmentRandomAccessOffsetBox::OnProcessData(const uint8_t* body, size_t length) {
    size = GetValue<uint32_t>(body);
    return SUCCESS;
}

M4AStatus TrackFragmentBaseMediaDecodeTimeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (_header.version == 1) baseMediaDecodeTime = GetValue<uint64_t>(body);
    else baseMediaDecodeTime = GetValue<uint32_t>(body);
    return SUCCESS;
}

M4AStatus LevelAssignmentBox::OnProcessData(const uint8_t* body, size_t length) {
    levelCount = GetValue<uint8_t>(body);
    if (levelCount == 0) return SUCCESS;
    uint32_t pos = 1;
    levelInfos   = std::make_unique<LevelInfo[]>(levelCount);
    for (uint32_t i = 0; i < levelCount; i++) {
        uint8_t byte                  = *(body + pos + 4);
        levelInfos[i].trackId         = GetValue<uint32_t>(body + pos);
        levelInfos[i].paddingFlag     = (byte >> 7) & 0x01;
        levelInfos[i].assignmentType  = byte & 0x7F;
        pos                          += 5;
        switch (levelInfos[i].assignmentType) {
            case 0:
                ReadData(body, levelInfos[i].groupingType, pos);
                break;
            case 1:
                ReadData(body, levelInfos[i].groupingType, pos);
                ReadData(body, levelInfos[i].groupingTypeParameter, pos);
                break;
            case 4:
                ReadData(body, levelInfos[i].subTrackId, pos);
                break;
            case 2:
            case 3:
            default:
                break;
        }
    }
    return SUCCESS;
}

M4AStatus TrackExtensionPropertiesBox::OnProcessData(const uint8_t* body, size_t length) {
    trackId = GetValue<uint32_t>(body);
    return SUCCESS;
}

M4AStatus AlternativeStartupSequencePropertiesBox::OnProcessData(const uint8_t* body, size_t length) {
    if (_header.version == 0) {
        minInitialAltStartupOffset = GetValue<int32_t>(body);
        return SUCCESS;
    }
    // version == 1
    count = GetValue<uint32_t>(body);
    if (count == 0) return SUCCESS;
    groupingTypeParameters      = std::make_unique<uint32_t[]>(count);
    minInitialAltStartupOffsets = std::make_unique<int32_t[]>(count);
    for (uint32_t i = 0; i < count; i++) {
        groupingTypeParameters[i]      = GetValue<uint32_t>(body + 4 + 8 * i);
        minInitialAltStartupOffsets[i] = GetValue<int32_t>(body + 8 + 8 * i);
    }
    return SUCCESS;
}
} // namespace myUtil
