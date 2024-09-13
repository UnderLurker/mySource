//
// Created by 常笑男 on 2024/8/11.
//
#include "webp.h"

#include <fstream>

#include "Util.h"

namespace myUtil {
namespace {
constexpr uint32_t FOURCC = 4;
constexpr uint32_t RIFF   = 0x52494646;
constexpr uint32_t WEBP   = 0x57454250;
constexpr uint32_t VPB    = 0x56503820;
uint64_t getByte(const uint8_t* data, int32_t count) {
    if (count > 8) return 0;
    uint64_t result = 0;
    for (int32_t i = 0; i < count; i++) {
        result = (result << 8) | data[i];
    }
    return result;
}
} // namespace

ImageStatus WebPData::read(const char* filePath) {
    CHECK_NULL_RETURN(filePath, ERROR_NULLPTR)
    std::fstream file(filePath, std::ios::in | std::ios::binary);
    if (file.fail()) return ERROR_FILE_OPERATOR;
    file.seekg(ios::end);
    auto size = file.tellg();
    _cache    = new uint8_t[size + streampos(1)];
    file.read((char*)_cache, size);
    CHECK_NULL_RETURN(_cache, ERROR_FILE_OPERATOR);
    processChunk();
    return SUCCESS;
}

ImageStatus WebPData::processChunk() {
    auto riff = getByte(_cache + _position, FOURCC);
    _position += 4;
    _fileSize = getByte(_cache + _position, 4);
    _position += 4;
    auto webp = getByte(_cache + _position, FOURCC);
    _position += 4;
    if (riff != RIFF || webp != WEBP) return ERROR_FILE_FORMAT;

    return SUCCESS;
}
} // namespace myUtil
