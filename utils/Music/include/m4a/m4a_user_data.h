//
// Created by 常笑男 on 2025/4/12.
//

#ifndef _M4A_USER_DATA_H
#define _M4A_USER_DATA_H

#include "m4a/m4a_common.h"

namespace myUtil {
// UDTA
struct UserDataBox : public Box {};
// CPRT
struct CopyrightBox : public LeafFullBox {
    uint16_t language; // ISO-639-2/T language code
    std::string notice;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// TSEL
struct TrackSelectionBox : public LeafFullBox {
    int32_t switchGroup;
    std::unique_ptr<uint32_t[]> attributeList;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// KIND
struct KindBox : public LeafFullBox {
    std::string schemeURI;
    std::string value;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
} // namespace myUtil

#endif // _M4A_USER_DATA_H
