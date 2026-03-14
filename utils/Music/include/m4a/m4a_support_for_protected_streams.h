//
// Created by 常笑男 on 2026/3/14.
//

#ifndef _M4A_SUPPORT_FOR_PROTECTED_STREAMS_H
#define _M4A_SUPPORT_FOR_PROTECTED_STREAMS_H

#include "m4a/m4a_common.h"

#include <memory>

namespace myUtil {
// SINF
struct ProtectionSchemeInformationBox : public Box {};
// FRMA
struct OriginalFormatBox : public Box {
    uint32_t dataFormat; // e.g. 'mp4v'
    bool ProcessFullBox(std::fstream& file) override;
};
// SCHM
struct SchemeTypeBox : public LeafFullBox {
    uint32_t schemeType;                    // 4CC identifying the scheme
    uint32_t schemeVersion;
    std::unique_ptr<uint8_t[]> schemeUri;   // UTF-8 characters
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// SCHI
struct SchemeInformationBox : public Box {
    SubBoxMap SchemeSpecificData() { return _boxes; }
};
} // namespace myUtil

#endif // _M4A_SUPPORT_FOR_PROTECTED_STREAMS_H
