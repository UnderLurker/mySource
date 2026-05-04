//
// Created by 常笑男 on 2025/4/20.
//

#include "m4a/m4a_post_decoder_requirements_on_media.h"

namespace myUtil {
bool StereoVideoBox::ProcessFullBox(std::fstream& file) {
    FullBox::ProcessFullBox(file);
    uint8_t buffer[12] = {0};
    file.read((char*)buffer, 12);
    info                  = GetValue<uint32_t>(buffer);
    stereoScheme          = GetValue<uint32_t>(buffer + 4);
    length                = GetValue<uint32_t>(buffer + 8);
    stereoIndicationTypes = std::make_unique<uint8_t[]>(length);
    file.read((char*)stereoIndicationTypes.get(), length);
    ProcessChildBox(file, _header.BodySize() - 4 - 12 - length);
    return SUCCESS;
}
} // namespace myUtil
