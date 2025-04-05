//
// Created by 常笑男 on 2025/4/4.
//

#ifndef _M4A_TRACK_DATA_LAYOUT_H
#define _M4A_TRACK_DATA_LAYOUT_H

#include "m4a/m4a_common.h"

namespace myUtil {
// DINF
struct DataInformationBox : public Box {};
// URL
struct DataEntryUrlBox : public LeafFullBox { // type = 'url '
    std::string location;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// URN
struct DataEntryUrnBox : public LeafFullBox { // type = 'urn '
    std::string name;
    std::string location;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// DREF
struct DataReferenceBox : public FullBox {
    M4AStatus ProcessData(std::fstream& file) override;
};
// STSZ
struct SampleSizeBox : public LeafFullBox {
    uint32_t sampleSize;
    uint32_t sampleCount;
    std::unique_ptr<uint32_t[]> entrySize;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// STZ2
struct CompactSampleSizeBox : public LeafFullBox {
    uint8_t fieldSize;
    uint32_t sampleCount;
    std::unique_ptr<uint8_t[]> entrySize;
    // 未实现获取指定位置的entrySize
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// STSC
struct SampleToChunkBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> firstChunk;
    std::unique_ptr<uint32_t[]> samplesPerChunk;
    std::unique_ptr<uint32_t[]> sampleDescriptionIndex;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// STCO
struct ChunkOffsetBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> chunkOffset;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// CO64
struct ChunkLargeOffsetBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint64_t[]> chunkOffset;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// PADB
struct PaddingBitsBox : public LeafFullBox {
    uint32_t sampleCount;
    struct PadInfo {
        uint8_t reserved1 : 1;
        // the number of bits at the end of sample (i*2)+1.
        uint8_t pad1      : 3;
        uint8_t reserved2 : 1;
        // the number of bits at the end of sample (i*2)+2.
        uint8_t pad2      : 3;
    };
    std::unique_ptr<PadInfo[]> padInfos;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// SUBS
struct SubSampleInformationBox : public LeafFullBox {
    struct SubSampleInfo {
        uint32_t size;
        uint8_t priority;
        uint8_t discard;
        uint32_t parameters;
    };
    struct SubSampleEntry {
        uint32_t delta;
        uint16_t count;
        std::unique_ptr<SubSampleInfo[]> infos;
    };
    uint32_t count;
    std::unique_ptr<SubSampleEntry[]> entry;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// SAIZ
struct SampleAuxiliaryInformationSizesBox : public LeafFullBox {
    uint32_t auxInfoType;
    uint32_t auxInfoTypeParameter;
    uint8_t defaultSampleInfoSize;
    uint32_t count;
    std::unique_ptr<uint8_t[]> infoSize;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// SAIO
struct SampleAuxiliaryInformationOffsetsBox : public LeafFullBox {
    uint32_t auxInfoType;
    uint32_t auxInfoTypeParameter;
    uint32_t count;
    std::unique_ptr<uint32_t[]> offset32;
    std::unique_ptr<uint64_t[]> offset64;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_TRACK_DATA_LAYOUT_H
