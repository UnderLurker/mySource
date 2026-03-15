//
// Created by 常笑男 on 2026/3/14.
//
#include "m4a/m4a_file_delivery_format_support.h"

#include <cstring>
#include <string>

namespace myUtil {
bool FDItemInformationBox::ProcessFullBox(std::fstream& file) {
    FullBox::ProcessFullBox(file);
    auto* t = new uint8_t[sizeof(uint16_t)];
    file.read((char*)t, sizeof(uint16_t));
    // PartitionEntryCount = GetValue<uint16_t>(t);
    delete[] t;
    return true;
}

M4AStatus FilePartitionBox::OnProcessData(const uint8_t* body, size_t length) {
    int32_t pos = 0;
    if (_header.version == 0) {
        itemId  = GetValue<uint16_t>(body);
        pos    += 2;
    } else {
        itemId  = GetValue<uint32_t>(body);
        pos    += 4;
    }
    packetPayloadSize           = GetValue<uint16_t>(body + pos);
    reserved                    = GetValue<uint8_t>(body + pos + 2);
    FECEncodingId               = GetValue<uint8_t>(body + pos + 3);
    FECInstanceId               = GetValue<uint16_t>(body + pos + 4);
    maxSourceBlockLength        = GetValue<uint16_t>(body + pos + 6);
    encodingSymbolLength        = GetValue<uint16_t>(body + pos + 8);
    maxNumberOfEncodingSymbols  = GetValue<uint16_t>(body + pos + 10);
    pos                        += 12;
    entryCount                  = 0;
    for (int32_t i = 0; i < length - pos; i++) {
        if (*(body + pos + i) == '\0') {
            schemeSpecificInfo  = std::string((char*)(body + pos), i);
            pos                += i;
            break;
        }
    }
    if (_header.version == 0) {
        entryCount  = GetValue<uint16_t>(body + pos);
        pos        += 2;
    } else {
        entryCount  = GetValue<uint32_t>(body + pos);
        pos        += 4;
    }
    if (entryCount != 0) {
        blockCount = std::make_unique<uint16_t[]>(entryCount);
        blockSize  = std::make_unique<uint32_t[]>(entryCount);
    }
    for (int32_t i = 0; i < entryCount; i++) {
        blockCount[i] = GetValue<uint16_t>(body + pos + 6 * i);
        blockSize[i]  = GetValue<uint32_t>(body + pos + 6 * i + 2);
    }
    return SUCCESS;
}

M4AStatus FECReservoirBox::OnProcessData(const uint8_t* body, size_t length) {
    int32_t pos = 0;
    if (_header.version == 0) {
        entryCount  = GetValue<uint16_t>(body);
        pos        += 2;
    } else {
        entryCount  = GetValue<uint32_t>(body);
        pos        += 4;
    }
    itemIds = std::make_unique<uint8_t[]>(entryCount * (_header.version == 0 ? 2 : 4));
    for (int32_t i = 0; i < entryCount; i++) {
        if (_header.version == 0) {
            uint16_t t         = GetValue<uint16_t>(body + pos + 6 * i);
            itemIds[2 * i]     = t >> 4;
            itemIds[2 * i + 1] = t & 0x0F;
        } else {
            uint32_t t         = GetValue<uint32_t>(body + pos + 8 * i);
            itemIds[4 * i]     = t >> 12;
            itemIds[4 * i + 1] = (t >> 8) & 0x000F;
            itemIds[4 * i + 2] = (t >> 4) & 0x000F;
            itemIds[4 * i + 3] = t & 0x000F;
        }
        int32_t offset  = (_header.version == 0 ? 6 : 8) * i + (_header.version == 0 ? 2 : 4);
        symbolCounts[i] = GetValue<uint32_t>(body + pos + offset);
    }
    return SUCCESS;
}

M4AStatus FDSessionGroupBox::OnProcessData(const uint8_t* body, size_t length) {
    int32_t pos                 = 0;
    numSessionGroups            = GetValue<uint16_t>(body);
    pos                        += 2;
    entryCounts                 = std::make_unique<uint8_t[]>(numSessionGroups);
    numChannelsInSessionGroups  = std::make_unique<uint16_t[]>(numSessionGroups);
    groupIds.reserve(numSessionGroups);
    hintTrackId.reserve(numSessionGroups);
    for (uint16_t i = 0; i < numSessionGroups; i++) {
        entryCounts[i] = GetValue<uint8_t>(body + pos);
        pos++;
        auto tmpGroupIds = std::make_unique<uint32_t[]>(entryCounts[i]);
        for (uint8_t j = 0; j < entryCounts[i]; j++) {
            tmpGroupIds[j] = GetValue<uint32_t>(body + pos + 4 * j);
        }
        pos += entryCounts[i] * 4;
        groupIds.push_back(std::move(tmpGroupIds));
        numChannelsInSessionGroups[i]  = GetValue<uint16_t>(body + pos);
        pos                           += 2;
        auto tmpHintTrackId            = std::make_unique<uint32_t[]>(numChannelsInSessionGroups[i]);
        for (uint16_t k = 0; k < numChannelsInSessionGroups[i]; k++) {
            tmpHintTrackId[k] = GetValue<uint32_t>(body + pos + 4 * k);
        }
        pos += numChannelsInSessionGroups[i] * 4;
        hintTrackId.push_back(std::move(tmpHintTrackId));
    }
    return SUCCESS;
}

M4AStatus GroupIdToNameBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount  = 0;
    entryCount  = GetValue<uint16_t>(body);
    int32_t pos = 2;
    groupIds    = std::make_unique<uint32_t[]>(entryCount);
    groupNames.reserve(entryCount);
    for (uint16_t i = 0; i < entryCount; i++) {
        groupIds[i]  = GetValue<uint32_t>(body + pos);
        pos         += 4;
        for (int32_t j = 0; j < length - pos; j++) {
            if (*(body + pos + j) == '\0') {
                groupNames.emplace_back((char*)(body + pos), j);
                pos += j;
                break;
            }
        }
    }
    return SUCCESS;
}

M4AStatus FileReservoirBox::OnProcessData(const uint8_t* body, size_t length) {
    return SUCCESS;
}
} // namespace myUtil
