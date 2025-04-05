//
// Created by 常笑男 on 2025/4/4.
//

#ifndef _M4A_GENERAL_H
#define _M4A_GENERAL_H

#include "m4a/m4a_common.h"

namespace myUtil {
// MDAT
struct MediaDataBox : public LeafBox {
    std::unique_ptr<uint8_t[]> data;
    size_t len {0};
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};

// Free Space Box (Can ignore)
struct FreeBox : public MediaDataBox {};
// Free Space Box (Can ignore)
struct SkipBox : public MediaDataBox {};

// PDIN
struct ProgressiveDownloadInfoBox : public LeafBox {
    struct PdinInfo {
        // a download rate expressed in bytes/second
        uint32_t rate;
        // InitialDelay is the suggested delay to use when playing the file, such that if download continues at the
        // given rate, all data within the file will arrive in time for its use and playback should not need to stall.
        uint32_t initialDelay;
    };
    std::list<PdinInfo> infos;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_GENERAL_H
