//
// Created by 常笑男 on 2025/4/5.
//

#ifndef _M4A_MOVIE_FRAGMENTS_H
#define _M4A_MOVIE_FRAGMENTS_H

#include "m4a/m4a_common.h"

namespace myUtil {
// MVEX
struct MovieExtendsBox : public Box {};
// MEHD
struct MovieExtendsHeaderBox : public LeafFullBox {
    uint64_t fragmentDuration;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// TREX
struct TrackExtendsBox : public LeafFullBox {
    struct SampleFlag {
        uint16_t reserved            : 4;
        uint16_t isLeading           : 2;
        uint16_t dependsOn           : 2;
        uint16_t isDependedOn        : 2;
        uint16_t hasRedundancy       : 2;
        uint16_t paddingValue        : 3;
        uint16_t isNonSyncSample     : 1;
        uint16_t degradationPriority : 16;
    };
    uint32_t trackId;
    uint32_t descriptionIndex;
    uint32_t duration;
    uint32_t size;
    SampleFlag flags;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// MOOF
struct MovieFragmentBox : public Box {};
// MFHD
struct MovieFragmentHeaderBox : public LeafFullBox {
    uint32_t sequenceNumber;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// TRAF
struct TrackFragmentBox : public Box {};
// TFHD
struct TrackFragmentHeaderBox : public LeafFullBox {
    uint32_t trackId;
    std::optional<uint64_t> baseDataOffset;
    std::optional<uint32_t> sampleDescriptionIndex;
    std::optional<uint32_t> defaultSampleDuration;
    std::optional<uint32_t> defaultSampleSize;
    std::optional<uint32_t> defaultSampleFlags;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// TRUN
struct TrackRunBox : public LeafFullBox {
    struct TrackRunInfo {
        std::optional<uint32_t> duration;
        std::optional<uint32_t> size;
        std::optional<uint32_t> flags;
        std::optional<uint32_t> uCompositionTimeOffset;
        std::optional<int32_t> compositionTimeOffset;
    };
    uint32_t count;
    std::optional<int32_t> dataOffset;
    std::optional<uint32_t> firstSampleFlags;
    std::unique_ptr<TrackRunInfo[]> infos;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// MFRA
struct MovieFragmentRandomAccessBox : public Box {};
// TFRA
struct TrackFragmentRandomAccessBox : public LeafFullBox {
    struct LengthSize {
        uint8_t reserved  : 2;
        uint8_t trafNum   : 2;
        uint8_t trunNum   : 2;
        uint8_t sampleNum : 2;
    };
    struct LengthSizeEntry {
        std::optional<uint64_t> time64;       // version == 1
        std::optional<uint64_t> moofOffset64; // version == 1
        std::optional<uint32_t> time32;       // version != 1
        std::optional<uint32_t> moofOffset32; // version != 1
        uint32_t trafNumber;
        uint32_t trunNumber;
        uint32_t sampleNumber;
    };
    uint32_t trackId;
    LengthSize lengthSize;
    uint32_t entryCount;
    std::unique_ptr<LengthSizeEntry[]> entries;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// MFRO
struct MovieFragmentRandomAccessOffsetBox : public LeafFullBox {
    uint32_t size;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// TFDT
struct TrackFragmentBaseMediaDecodeTimeBox : public LeafFullBox {
    uint64_t baseMediaDecodeTime;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// LEVA
struct LevelAssignmentBox : public LeafFullBox {
    struct LevelInfo {
        uint32_t trackId;
        uint8_t paddingFlag    : 1;
        uint8_t assignmentType : 7;
        std::optional<uint32_t> groupingType;
        std::optional<uint32_t> groupingTypeParameter;
        std::optional<uint32_t> subTrackId;
    };
    uint8_t levelCount;
    std::unique_ptr<LevelInfo[]> levelInfos;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// TREP
struct TrackExtensionPropertiesBox : public LeafFullBox {
    uint32_t trackId;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// ASSP
struct AlternativeStartupSequencePropertiesBox : public LeafFullBox {
    int32_t minInitialAltStartupOffset;                     // version == 0
    uint32_t count;                                         // version == 1
    std::unique_ptr<uint32_t[]> groupingTypeParameters;     // version == 1
    std::unique_ptr<int32_t[]> minInitialAltStartupOffsets; // version == 1
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_MOVIE_FRAGMENTS_H
