//
// Created by 常笑男 on 2026/4/20.
//

#ifndef _M4A_POST_DECODER_REQUIREMENTS_ON_MEDIA_H
#define _M4A_POST_DECODER_REQUIREMENTS_ON_MEDIA_H

#include "m4a/m4a_common.h"

namespace myUtil {
// RINF has OriginalFormatBox, SchemeTypeBox, SchemeInformationBox(optional)
struct RestrictedSchemeInfoBox : public Box {};
// STVI
struct StereoVideoBox : public FullBox {
    uint32_t reserved() { return info >> 2; }
    uint8_t singleViewAllowed() { return info & 0x0003; }
    uint32_t info;
    uint32_t stereoScheme;
    uint32_t length;
    std::unique_ptr<uint8_t[]> stereoIndicationTypes;
    bool ProcessFullBox(std::fstream& file) override;
};
} // namespace myUtil

#endif // !_M4A_POST_DECODER_REQUIREMENTS_ON_MEDIA_H
