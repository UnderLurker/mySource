//
// Created by 常笑男 on 2026/4/20.
//

#ifndef _M4A_SUB_TRACKS_H
#define _M4A_SUB_TRACKS_H

#include "m4a/m4a_common.h"

namespace myUtil {
// STRK
struct SubTrack : public Box {};
// STRI
struct SubTrackInformation : public LeafFullBox {
    int16_t switchGroup    = 0;
    int16_t alternateGroup = 0;
    uint32_t subTrackID    = 0;
    // descriptive
    // `tesc` The sub‐track can be temporally scaled.
    // `fgsc` The sub‐track can be scaled in terms of quality.
    // `cgsc` The sub‐track can be scaled in terms of quality.
    // `spsc` The sub‐track can be spatially scaled.
    // `resc` The sub‐track can be region‐of‐interest scaled.
    // `vmsc` The sub‐track can be scaled in terms of number of views.
    // different
    // `bitr` Total size of the samples in the track divided by the duration in the track header box
    // `frar` Number of samples in the track divided by duration in the track header box
    // `nvws` Number of views in the sub track
    std::unique_ptr<uint32_t[]> attributeList;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// STRD
struct SubTrackDefinition : public Box {};
// STSG
struct SubTrackSampleGroupBox : public LeafFullBox {
    uint32_t groupingType;
    uint16_t itemCount;
    std::unique_ptr<uint32_t[]> groupDescriptionIndexs;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // !_M4A_SUB_TRACKS_H
