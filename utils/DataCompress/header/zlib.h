//
// Created by 常笑男 on 2024/2/10.
//
#ifndef _ZLIB_H
#define _ZLIB_H

#include "Util.h"
#include "compress_base.h"

NAME_SPACE_START(myUtil)

class Zlib : CompressBase<uint8_t> {
public:
    CompressStatus decode(const uint8_t* source, uint32_t sLength, uint8_t* result, uint32_t& rLength) override;
    CompressStatus encode(const uint8_t* source, uint32_t sLength, uint8_t* result, uint32_t& rLength) override;
};

NAME_SPACE_END()

#endif // !_ZLIB_H
