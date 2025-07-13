//
// Created by 常笑男 on 2025/4/4.
//
#include "m4a/m4a_track_data_layout.h"

#include <cstring>

namespace myUtil {

M4AStatus DataEntryUrlBox::OnProcessData(const uint8_t* body, size_t length) {
    location = std::string(length, STRING_FILL);
    memcpy(location.data(), body, length);
    return SUCCESS;
}

M4AStatus DataEntryUrnBox::OnProcessData(const uint8_t* body, size_t length) {
    size_t pos = length;
    for (size_t i = 0; i < length; i++) {
        if (*(body + i) == '\0') {
            pos = i;
            break;
        }
    }
    name     = std::string(pos, STRING_FILL);
    location = std::string(length - pos - 1, STRING_FILL);
    memcpy(name.data(), body, pos);
    memcpy(location.data(), body + pos + 1, length - pos - 1);
    return SUCCESS;
}

M4AStatus DataReferenceBox::ProcessData(std::fstream& file) {
    file.seekg(sizeof(uint32_t), std::ios::cur);
    return FullBox::ProcessData(file);
}

M4AStatus SampleSizeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length < 8) return ERROR_FILE_FORMAT;
    sampleSize  = GetValue<uint32_t>(body);
    sampleCount = GetValue<uint32_t>(body + 4);
    if (sampleSize != 0) return SUCCESS;
    entrySize = std::make_unique<uint32_t[]>(sampleCount);
    for (size_t i = 0; i < sampleCount; i++) {
        entrySize[i] = GetValue<uint32_t>(body + 8 + 4 * i);
    }
    return SUCCESS;
}

M4AStatus CompactSampleSizeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length < 8) return ERROR_FILE_FORMAT;
    fieldSize   = GetValue<uint32_t>(body) & 0x000000FF;
    sampleCount = GetValue<uint32_t>(body + 4);
    if (length == 8) return SUCCESS;
    entrySize = std::make_unique<uint8_t[]>(length - 8);
    memcpy(entrySize.get(), body + 8, length - 8);
    return SUCCESS;
}

M4AStatus SampleToChunkBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount             = GetValue<uint32_t>(body);
    firstChunk             = std::make_unique<uint32_t[]>(entryCount);
    samplesPerChunk        = std::make_unique<uint32_t[]>(entryCount);
    sampleDescriptionIndex = std::make_unique<uint32_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        firstChunk[i]             = GetValue<uint32_t>(body + 4 + 12 * i);
        samplesPerChunk[i]        = GetValue<uint32_t>(body + 8 + 12 * i);
        sampleDescriptionIndex[i] = GetValue<uint32_t>(body + 12 + 12 * i);
    }
    return SUCCESS;
}

M4AStatus ChunkOffsetBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount  = GetValue<uint32_t>(body);
    chunkOffset = std::make_unique<uint32_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        chunkOffset[i] = GetValue<uint32_t>(body + 4 + 4 * i);
    }
    return SUCCESS;
}

M4AStatus ChunkLargeOffsetBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount  = GetValue<uint32_t>(body);
    chunkOffset = std::make_unique<uint64_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        chunkOffset[i] = GetValue<uint64_t>(body + 4 + 8 * i);
    }
    return SUCCESS;
}

M4AStatus PaddingBitsBox::OnProcessData(const uint8_t* body, size_t length) {
    sampleCount  = GetValue<uint32_t>(body);
    uint32_t len = (sampleCount + 1) / 2;
    padInfos     = std::make_unique<PadInfo[]>(len);
    for (uint32_t i = 0; i < len; i++) {
        auto temp   = GetValue<uint8_t>(body + 4 + i);
        padInfos[i] = {.reserved1 = 0,
                       .pad1      = static_cast<uint8_t>((temp >> 4) & 0b00000111),
                       .reserved2 = 0,
                       .pad2      = static_cast<uint8_t>(temp & 0b00000111)};
    }
    return SUCCESS;
}

M4AStatus SubSampleInformationBox::OnProcessData(const uint8_t* body, size_t length) {
    count        = GetValue<uint32_t>(body);
    entry        = std::make_unique<SubSampleEntry[]>(count);
    uint32_t pos = 4;
    for (uint32_t i = 0; i < count; i++) {
        auto& tmp  = entry[i];
        tmp.delta  = GetValue<uint32_t>(body + pos);
        tmp.count  = GetValue<uint16_t>(body + pos + 4);
        pos       += 4 + 2;
        if (tmp.count == 0) continue;
        tmp.infos = std::make_unique<SubSampleInfo[]>(tmp.count);
        for (uint32_t j = 0; j < tmp.count; j++) {
            if (_header.version == 1) {
                tmp.infos[j].size  = GetValue<uint32_t>(body + pos);
                pos               += 4;
            } else {
                tmp.infos[j].size  = GetValue<uint16_t>(body + pos);
                pos               += 2;
            }
            tmp.infos[j].priority    = GetValue<uint8_t>(body + pos);
            tmp.infos[j].discard     = GetValue<uint8_t>(body + pos + 1);
            tmp.infos[j].parameters  = GetValue<uint32_t>(body + pos + 2);
            pos                     += 6;
        }
    }
    return SUCCESS;
}

M4AStatus SampleAuxiliaryInformationSizesBox::OnProcessData(const uint8_t* body, size_t length) {
    uint32_t pos = 0;
    if (_header.flags & 1) {
        auxInfoType           = GetValue<uint32_t>(body);
        auxInfoTypeParameter  = GetValue<uint32_t>(body + 4);
        pos                  += 8;
    }
    defaultSampleInfoSize = GetValue<uint8_t>(body + pos);
    count                 = GetValue<uint32_t>(body + pos + 1);
    if (count == 0) return SUCCESS;
    infoSize = std::make_unique<uint8_t[]>(count);
    memcpy(infoSize.get(), body + pos + 5, count);
    return SUCCESS;
}

M4AStatus SampleAuxiliaryInformationOffsetsBox::OnProcessData(const uint8_t* body, size_t length) {
    uint32_t pos = 0;
    if (_header.flags & 1) {
        auxInfoType           = GetValue<uint32_t>(body);
        auxInfoTypeParameter  = GetValue<uint32_t>(body + 4);
        pos                  += 8;
    }
    count  = GetValue<uint32_t>(body + pos);
    pos   += 4;
    if (count == 0) return SUCCESS;
    if (_header.version == 0) {
        offset32 = std::make_unique<uint32_t[]>(count);
    } else {
        offset64 = std::make_unique<uint64_t[]>(count);
    }
    for (uint32_t i = 0; i < count; i++) {
        if (_header.version == 0) {
            offset32[i] = GetValue<uint32_t>(body + pos + 4 * i);
        } else {
            offset64[i] = GetValue<uint64_t>(body + pos + 8 * i);
        }
    }
    return SUCCESS;
}
} // namespace myUtil
