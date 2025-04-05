//
// Created by 常笑男 on 2025/4/4.
//

#ifndef _M4A_TRACK_H
#define _M4A_TRACK_H

#include "m4a/m4a_common.h"

namespace myUtil {
// TRAK
struct TrackBox : public Box {};
// TKHD
struct TrackHeaderBox : public LeafFullBox {
    uint64_t creationTime;
    uint64_t modificationTime;
    uint32_t trackId;
    uint32_t reserved {0};
    uint64_t duration;
    int16_t layer {0};
    int16_t alternateGroup {0};
    int16_t volume; // if track_is_audio 0x0100 else 0
    int32_t matrix[9] {0x00010000, 0, 0, 0, 0x00010000, 0, 0, 0, 0x40000000};
    uint32_t width;
    uint32_t height;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
    [[nodiscard]] bool isAudio() const { return volume == 0x0100; }
};
// TREF
struct TrackReferenceTypeBox : public LeafBox {
    std::unique_ptr<uint32_t[]> trackIDs;
    size_t len {0};
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// TRGR
struct TrackGroupTypeBox : public LeafBox {
    uint32_t trackGroupId;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_TRACK_H
