//
// Created by 常笑男 on 2025/4/4.
//

#ifndef _M4A_TRACK_MEDIA_H
#define _M4A_TRACK_MEDIA_H

#include "m4a/m4a_common.h"

namespace myUtil {
// MDIA
struct MediaBox : public Box {};
// MDHD
struct MediaHeaderBox : public LeafFullBox {
    uint64_t creationTime;
    uint64_t modificationTime;
    uint32_t timeScale;
    uint64_t duration;
    bool pad {false};
    uint16_t language; // ISO-639-2/T language Code - Bit(15)
    uint16_t preDefined {0};
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// HDLR
struct HandlerBox : public LeafFullBox {
    uint32_t preDefined {0};
    uint32_t handlerType;
    uint32_t reserved[3] {0};
    std::string name;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// MINF
struct MediaInformationBox : public Box {};
// NMHD
struct NullMediaHeaderBox : public FullBox {};
// ELNG
struct ExtendedLanguageBox : public LeafFullBox {
    std::string extendedLanguage;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_TRACK_MEDIA_H
