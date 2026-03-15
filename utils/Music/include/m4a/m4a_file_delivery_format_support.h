//
// Created by 常笑男 on 2026/3/14.
//

#ifndef _M4A_FILE_DELIVERY_FORMAT_SUPPORT_H
#define _M4A_FILE_DELIVERY_FORMAT_SUPPORT_H

#include "m4a/m4a_common.h"

#include <memory>

namespace myUtil {
// PAEN
struct PartitionEntry : public Box {};
// FIIN
struct FDItemInformationBox : public FullBox {
    bool ProcessFullBox(std::fstream& file) override;
};
// fpar
struct FilePartitionBox : public LeafFullBox {
    uint32_t itemId;
    uint16_t packetPayloadSize;
    uint8_t reserved {0};
    uint8_t FECEncodingId;
    uint16_t FECInstanceId;
    uint16_t maxSourceBlockLength;
    uint16_t encodingSymbolLength;
    uint16_t maxNumberOfEncodingSymbols;
    std::string schemeSpecificInfo;
    uint32_t entryCount;
    std::unique_ptr<uint16_t[]> blockCount;
    std::unique_ptr<uint32_t[]> blockSize;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// FECR
struct FECReservoirBox : public LeafFullBox {
    uint32_t entryCount {0};
    std::unique_ptr<uint8_t[]> itemIds;
    std::unique_ptr<uint32_t[]> symbolCounts;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// SEGR
struct FDSessionGroupBox : public LeafBox {
    uint16_t numSessionGroups;
    std::unique_ptr<uint8_t[]> entryCounts;
    std::vector<std::unique_ptr<uint32_t[]>> groupIds;
    std::unique_ptr<uint16_t[]> numChannelsInSessionGroups;
    std::vector<std::unique_ptr<uint32_t[]>> hintTrackId;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// GITN
struct GroupIdToNameBox : public LeafFullBox {
    uint16_t entryCount;
    std::unique_ptr<uint32_t[]> groupIds;
    std::vector<std::string> groupNames;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// FIRE
struct FileReservoirBox : public LeafFullBox {
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_FILE_DELIVERY_FORMAT_SUPPORT_H
