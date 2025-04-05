//
// Created by 常笑男 on 2025/4/4.
//

#ifndef _M4A_MOVIE_H
#define _M4A_MOVIE_H

#include "m4a/m4a_common.h"

namespace myUtil {
// MOOV
struct MovieBox : public Box {};
// MVHD
struct MovieHeaderBox : public LeafFullBox {
    uint64_t createTime;
    uint64_t modificationTime;
    uint32_t timeScale;
    uint64_t duration;
    uint32_t rate {0x00010000};
    int16_t volume {0x0100};
    uint8_t reserved[10] {0};
    int32_t matrix[9] {0x00010000, 0, 0, 0, 0x00010000, 0, 0, 0, 0x40000000};
    int32_t preDefined[6] {0};
    uint32_t nextTrackId;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};

} // namespace myUtil

#endif // _M4A_MOVIE_H
