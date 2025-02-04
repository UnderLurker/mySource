//
// Created by 常笑男 on 2024/8/11.
//
#include "webp.h"

#include <fstream>

#include "Util.h"

namespace myUtil {
namespace {
constexpr uint32_t FOURCC = 4;
constexpr uint32_t RIFF   = ('R' << 24) | ('I' << 16) | ('F' << 8) | 'F';
constexpr uint32_t WEBP   = ('W' << 24) | ('E' << 16) | ('B' << 8) | 'P';
constexpr uint32_t VP8    = ('V' << 24) | ('P' << 16) | ('8' << 8) | ' ';
constexpr uint32_t VP8L   = ('V' << 24) | ('P' << 16) | ('8' << 8) | 'L';
uint64_t getByte(uint8_t** data, int32_t count) {
    if (count > 8) return 0;
    uint64_t result = 0;
    for (int32_t i = 0; i < count; i++) {
        result = (result << 8) | *((*data)++);
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
    processData();
    return SUCCESS;
}

ImageStatus WebPData::processData() {
    auto headerStatus = processHeader();
    if (headerStatus) return headerStatus;

    return SUCCESS;
}

ImageStatus WebPData::processHeader() {
    auto tempPos = _cache + _position;
    auto riff = getByte(&tempPos, FOURCC);
    _fileSize = getByte(&tempPos, FOURCC);
    auto webp = getByte(&tempPos, FOURCC);
    auto vp8 = getByte(&tempPos, FOURCC);
    if (riff != RIFF || webp != WEBP || (vp8 != VP8 && vp8 != VP8L)) return ERROR_FILE_FORMAT;
    _loss = vp8 == VP8;
    _position += tempPos - _cache;
    return SUCCESS;
}
} // namespace myUtil
