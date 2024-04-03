#include "png.h"

#include <fstream>
#include <sstream>
#include <windef.h>
#include <zlib.h>

#include "ImageUtil.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

#define ANCILLARY_CHUNK_FACTORY(chunkName)                                            \
    if (myUtil::CHUNK::chunkName == head.chunkType) {                                 \
        Chunk* chunk                              = new chunkName##Chunk();           \
        auto result                               = chunk->decode(file, head.length); \
        _ancillaryChunk[myUtil::CHUNK::chunkName] = chunk;                            \
        return result;                                                                \
    }

#define ANCILLARY_CHUNK_FACTORY_FUNC(chunkName, statement)                            \
    if (myUtil::CHUNK::chunkName == head.chunkType) {                                 \
        Chunk* chunk = new chunkName##Chunk();                                        \
        statement;                                                                    \
        auto result                               = chunk->decode(file, head.length); \
        _ancillaryChunk[myUtil::CHUNK::chunkName] = chunk;                            \
        return result;                                                                \
    }

void convertSmall16(uint16_t& value) {
    uint16_t value1 = value & 0x00ff;
    uint16_t value2 = value & 0xff00;
    value           = (value1 << 8) | (value2 >> 8);
}

void convertSmall32(uint32_t& value) {
    uint32_t value1 = value & 0x000000ff;
    uint32_t value2 = value & 0x0000ff00;
    uint32_t value3 = value & 0x00ff0000;
    uint32_t value4 = value & 0xff000000;
    value           = (value1 << 24) | (value2 << 8) | (value3 >> 8) | (value4 >> 24);
}

uint8_t PaethPredictor(uint8_t a, uint8_t b, uint8_t c) {
    int32_t p  = a + b - c;
    int32_t pa = abs(p - a);
    int32_t pb = abs(p - b);
    int32_t pc = abs(p - c);
    if (pa <= pb && pa <= pc) return a;
    else if (pb <= pc) return b;
    else return c;
}

unsigned long Chunk::crcTable[256] {0};
bool Chunk::crcTableComputed = false;

[[maybe_unused]] uint32_t Chunk::createCRC(const uint8_t* buf, uint32_t len) {
    if (!crcTableComputed) {
        for (uint32_t n = 0; n < 256; n++) {
            auto c = (unsigned long)n;
            for (uint32_t k = 0; k < 8; k++) {
                if (c & 1) c = 0xedb88320L ^ (c >> 1);
                else c = c >> 1;
            }
            crcTable[n] = c;
        }
        crcTableComputed = true;
    }
    unsigned long c = 0xFFFFFFFFL;
    for (uint32_t n = 0; n < len; n++) {
        c = crcTable[(c ^ buf[n]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFFL;
}

void Chunk::write32(fstream& file, uint32_t value) {
    for (int32_t i = 0; i < 4; i++) {
        file << (uint8_t)((value & 0xFF000000) >> 24);
        value <<= 8;
    }
}

void Chunk::write32(uint8_t* target, uint32_t value, uint32_t pos) {
    for (int32_t i = 0; i < 4; i++) {
        target[pos + i] = (uint8_t)((value & 0xFF000000) >> 24);

        value <<= 8;
    }
}

void Chunk::write16(fstream& file, uint16_t value) {
    for (int32_t i = 0; i < 2; i++) {
        file << (uint8_t)((value & 0xFF00) >> 8);
        value <<= 8;
    }
}

void Chunk::write16(uint8_t* target, uint16_t value, uint32_t pos) {
    for (int32_t i = 0; i < 2; i++) {
        target[pos + i] = (uint8_t)((value & 0xFF00) >> 8);

        value <<= 8;
    }
}

ImageStatus IHDRChunk::decode(fstream& file, uint32_t length) {
    if (length <= 0) return ERROR_LIMIT;
    try {
        file.read((char*)&_data, length);
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(_data.width);
        convertSmall32(_data.height);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus IHDRChunk::encode(std::fstream& file) {
    try {
        uint32_t dataLen = 13;
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::IHDR].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(dataLen);
        write32(ss.get(), _data.width, 0);
        write32(ss.get(), _data.height, 4);
        ss[8]  = bitDepth();
        ss[9]  = colorType();
        ss[10] = compressionMethod();
        ss[11] = filterMethod();
        ss[12] = interlaceMethod();
        file.write((char*)ss.get(), dataLen);
        crc.crc = createCRC(ss.get(), dataLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

uint32_t IHDRChunk::minCompressDataLength() const {
    uint32_t result = 0;
    switch (colorType()) {
        case CHUNK::GREY_SCALE:
            result = CHUNK::ColorTypeBitMap[CHUNK::GREY_SCALE] * bitDepth() / 8;
            break;
        case CHUNK::RGB:
            result = CHUNK::ColorTypeBitMap[CHUNK::RGB] * bitDepth() / 8;
            break;
        case CHUNK::INDEXED_COLOUR:
            result = CHUNK::ColorTypeBitMap[CHUNK::INDEXED_COLOUR] * bitDepth() / 8;
            break;
        case CHUNK::GREY_ALPHA:
            result = CHUNK::ColorTypeBitMap[CHUNK::GREY_ALPHA] * bitDepth() / 8;
            break;
        case CHUNK::RGBA:
            result = CHUNK::ColorTypeBitMap[CHUNK::RGBA] * bitDepth() / 8;
            break;
    }
    return result;
}

ImageStatus PLTEChunk::decode(fstream& file, uint32_t length) {
    if (length % 3 != 0) return ERROR_FILE_FORMAT;
    try {
        uint32_t len = length / 3;
        _data.resize(len);
        for (uint32_t i = 0; i < len; i++) {
            PLTEData data {};
            file.read((char*)&data, sizeof(PLTEData));
            _data.at(i) = data;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus PLTEChunk::encode(std::fstream& file) {
    try {
        uint32_t dataLen = sizeof(PLTEData) * _data.size();
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::PLTE].c_str(), 4);
        auto ss      = std::make_unique<uint8_t[]>(dataLen);
        uint32_t pos = 0;
        for (const auto& item : _data) {
            ss[pos++] = item.rgbRed;
            ss[pos++] = item.rgbGreen;
            ss[pos++] = item.rgbBlue;
        }
        file.write((char*)ss.get(), dataLen);
        crc.crc = createCRC(ss.get(), dataLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus tRNSChunk::decode(fstream& file, uint32_t length) {
    try {
        switch (colourType) {
            case 0: {
                if (length != 2) return ERROR_FILE_FORMAT;
                file.read((char*)&_data.colourType0, sizeof(_data.colourType0));
                convertSmall16(_data.colourType0);
                break;
            }
            case 2: {
                if (length != 6) return ERROR_FILE_FORMAT;
                file.read((char*)&_data.colourType2.red, 2);
                file.read((char*)&_data.colourType2.green, 2);
                file.read((char*)&_data.colourType2.blue, 2);
                convertSmall16(_data.colourType2.red);
                convertSmall16(_data.colourType2.green);
                convertSmall16(_data.colourType2.blue);
                break;
            }
            case 3: {
                _data.colourType3.resize(length);
                for (uint32_t i = 0; i < length; i++) {
                    uint8_t index;
                    file.read((char*)&index, sizeof(index));
                    _data.colourType3.at(i) = index;
                }
                break;
            }
            default:
                return ERROR_ENUM;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus tRNSChunk::encode(std::fstream& file) {
    try {
        uint32_t size = 2;
        if (colourType == 2) size = 6;
        else if (colourType == 3) size = _data.colourType3.size();
        write32(file, size);
        file.write(CHUNK::typeMap[CHUNK::tRNS].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(size);
        switch (colourType) {
            case 0:
                write16(ss.get(), _data.colourType0, 0);
                break;
            case 2: {
                write16(ss.get(), _data.colourType2.red, 0);
                write16(ss.get(), _data.colourType2.green, 2);
                write16(ss.get(), _data.colourType2.blue, 4);
                break;
            }
            case 3: {
                uint32_t pos = 0;
                for_each(_data.colourType3.begin(), _data.colourType3.end(), [&](uint8_t val) { ss[pos++] = val; });
                break;
            }
            default:
                return ERROR_ENUM;
        }
        file.write((char*)ss.get(), size);
        crc.crc = createCRC(ss.get(), size);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus cHRMChunk::decode(fstream& file, uint32_t length) {
    if (length != sizeof(cHRMData)) return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&_data, sizeof(cHRMData));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(_data.whitePointX);
        convertSmall32(_data.whitePointY);
        convertSmall32(_data.redX);
        convertSmall32(_data.redY);
        convertSmall32(_data.greenX);
        convertSmall32(_data.greenY);
        convertSmall32(_data.blueX);
        convertSmall32(_data.blueY);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus cHRMChunk::encode(std::fstream& file) {
    try {
        uint32_t dataLen = 32;
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::cHRM].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(dataLen);
        write32(ss.get(), _data.whitePointX, 0);
        write32(ss.get(), _data.whitePointY, 4);
        write32(ss.get(), _data.redX, 8);
        write32(ss.get(), _data.redY, 12);
        write32(ss.get(), _data.greenX, 16);
        write32(ss.get(), _data.greenY, 20);
        write32(ss.get(), _data.blueX, 24);
        write32(ss.get(), _data.blueY, 28);
        file.write((char*)ss.get(), dataLen);
        crc.crc = createCRC(ss.get(), dataLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus gAMAChunk::decode(fstream& file, uint32_t length) {
    if (length != sizeof(_data)) return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(_data);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus gAMAChunk::encode(std::fstream& file) {
    try {
        uint32_t dataLen = 4;
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::gAMA].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(dataLen);
        write32(ss.get(), _data, 0);
        file.write((char*)ss.get(), dataLen);
        crc.crc = createCRC(ss.get(), dataLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus iCCPChunk::decode(fstream& file, uint32_t length) {
    try {
        char ch = 1;
        for (uint32_t i = 0; i < length; i++) {
            file.read((char*)&ch, sizeof(ch));
            if (ch == 0) {
                _data.profileName[i] = '\0';
                break;
            } else {
                _data.profileName[i] = ch;
            }
        }
        file.read((char*)&_data.compressMethod, sizeof(_data.compressMethod));
        uint32_t len = length - strlen(_data.profileName) - 2;
        _data.compressProfile.resize(len);
        for (uint32_t i = 0; i < len; i++) {
            uint8_t data;
            file.read((char*)&data, sizeof(data));
            _data.compressProfile.at(i) = data;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus iCCPChunk::encode(std::fstream& file) {
    try {
        uint32_t i       = 0;
        uint32_t dataLen = strlen(_data.profileName) + 2 + _data.compressProfile.size();
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::iCCP].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(dataLen);
        for (; i < strlen(_data.profileName); i++) {
            ss[i] = _data.profileName[i];
        }
        ss[i++] = 0;
        ss[i++] = _data.compressMethod;
        for (const auto& item : _data.compressProfile) {
            ss[i++] = item;
        }
        file.write((char*)ss.get(), dataLen);
        crc.crc = createCRC(ss.get(), dataLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus sBITChunk::decode(fstream& file, uint32_t length) {
    try {
        switch (colourType) {
            case CHUNK::GREY_SCALE:
                file.read((char*)&_data.colourType04.greyScale, 1);
                break;
            case CHUNK::RGB:
            case CHUNK::INDEXED_COLOUR: {
                file.read((char*)&_data.colourType236.red, 1);
                file.read((char*)&_data.colourType236.green, 1);
                file.read((char*)&_data.colourType236.blue, 1);
                break;
            }
            case CHUNK::GREY_ALPHA:
                file.read((char*)&_data.colourType04, 2);
                break;
            case CHUNK::RGBA: {
                file.read((char*)&_data.colourType236.red, 1);
                file.read((char*)&_data.colourType236.green, 1);
                file.read((char*)&_data.colourType236.blue, 1);
                file.read((char*)&_data.colourType236.alpha, 1);
                break;
            }
            default:
                break;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus sBITChunk::encode(std::fstream& file) {
    try {
        uint32_t size = 1;
        if (colourType == CHUNK::RGB || colourType == CHUNK::INDEXED_COLOUR) size = 3;
        else if (colourType == CHUNK::GREY_ALPHA) size = 2;
        else if (colourType == CHUNK::RGBA) size = 4;
        write32(file, size);
        file.write(CHUNK::typeMap[CHUNK::sBIT].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(size);
        switch (colourType) {
            case CHUNK::GREY_SCALE:
                ss[0] = _data.colourType04.greyScale;
                break;
            case CHUNK::RGB:
            case CHUNK::INDEXED_COLOUR: {
                ss[0] = _data.colourType236.red;
                ss[1] = _data.colourType236.green;
                ss[2] = _data.colourType236.blue;
                break;
            }
            case CHUNK::GREY_ALPHA: {
                ss[0] = _data.colourType04.greyScale;
                ss[1] = _data.colourType04.alpha;
                break;
            }
            case CHUNK::RGBA: {
                ss[0] = _data.colourType236.red;
                ss[1] = _data.colourType236.green;
                ss[2] = _data.colourType236.blue;
                ss[3] = _data.colourType236.alpha;
                break;
            }
            default:
                break;
        }
        file.write((char*)ss.get(), size);
        crc.crc = createCRC(ss.get(), size);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus sRGBChunk::decode(fstream& file, uint32_t length) {
    if (length != sizeof(_data)) return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus sRGBChunk::encode(std::fstream& file) {
    try {
        uint32_t dataLen = 1;
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::sRGB].c_str(), 4);
        file.write((char*)&_data, dataLen);
        crc.crc = createCRC(&_data, 1);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus cICPChunk::decode(fstream& file, uint32_t length) {
    if (length != sizeof(_data)) return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus cICPChunk::encode(std::fstream& file) {
    try {
        uint32_t dataLen = 4;
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::cICP].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(dataLen);
        ss[0]   = _data.colourPrimaries;
        ss[1]   = _data.transferFunction;
        ss[2]   = _data.matrixCoefficients;
        ss[3]   = _data.videoFullRangeFlag;
        file.write((char*)ss.get(), dataLen);
        crc.crc = createCRC(ss.get(), dataLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus mDCvChunk::decode(fstream& file, uint32_t length) {
    if (length != sizeof(_data)) return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall16(_data.chromaticityX1);
        convertSmall16(_data.chromaticityY1);
        convertSmall16(_data.chromaticityX2);
        convertSmall16(_data.chromaticityY2);
        convertSmall16(_data.chromaticityX3);
        convertSmall16(_data.chromaticityY3);
        convertSmall16(_data.whitePointX);
        convertSmall16(_data.whitePointY);
        convertSmall32(_data.maxLum);
        convertSmall32(_data.minLum);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus mDCvChunk::encode(std::fstream& file) {
    try {
        uint32_t dataLen = 24;
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::mDCv].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(dataLen);
        write16(ss.get(), _data.chromaticityX1, 0);
        write16(ss.get(), _data.chromaticityY1, 2);
        write16(ss.get(), _data.chromaticityX2, 4);
        write16(ss.get(), _data.chromaticityY2, 6);
        write16(ss.get(), _data.chromaticityX3, 8);
        write16(ss.get(), _data.chromaticityY3, 10);
        write16(ss.get(), _data.whitePointX, 12);
        write16(ss.get(), _data.whitePointY, 14);
        write32(ss.get(), _data.maxLum, 16);
        write32(ss.get(), _data.minLum, 20);
        file.write((char*)ss.get(), dataLen);
        crc.crc = createCRC(ss.get(), dataLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

vector<mDCvChunk::Chromaticity> mDCvChunk::chromaticity() const {
    return {
        {_data.chromaticityX1 * 0.00002f, _data.chromaticityY1 * 0.00002f},
        {_data.chromaticityX2 * 0.00002f, _data.chromaticityY2 * 0.00002f},
        {_data.chromaticityX3 * 0.00002f, _data.chromaticityY3 * 0.00002f},
    };
}

mDCvChunk::Chromaticity mDCvChunk::whitePoint() { return {_data.whitePointX * 0.00002f, _data.whitePointY * 0.00002f}; }

float mDCvChunk::maxLum() { return _data.maxLum * 0.0001; }

float mDCvChunk::minLum() { return _data.minLum * 0.0001; }

ImageStatus cLLiChunk::decode(fstream& file, uint32_t length) {
    if (length != sizeof(uint32_t) * 2) return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&MaxCLL, sizeof(MaxCLL));
        file.read((char*)&MaxFALL, sizeof(MaxFALL));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(MaxCLL);
        convertSmall32(MaxFALL);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus cLLiChunk::encode(std::fstream& file) {
    try {
        uint32_t dataLen = 8;
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::cLLi].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(dataLen);
        write32(ss.get(), MaxCLL, 0);
        write32(ss.get(), MaxFALL, 4);
        file.write((char*)ss.get(), dataLen);
        crc.crc = createCRC(ss.get(), dataLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus tEXtChunk::decode(fstream& file, uint32_t length) {
    try {
        for (uint32_t i = 0; i < length; i++) {
            char ch;
            file.read((char*)&ch, sizeof(ch));
            if (ch == 0) {
                keyWord[i] = '\0';
                break;
            } else {
                keyWord[i] = ch;
            }
        }
        uint32_t len = length - strlen(keyWord) - 1;
        textString.resize(len + 1);
        for (uint32_t i = 0; i < len; i++) {
            char ch;
            file.read((char*)&ch, sizeof(ch));
            textString[i] = ch;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus tEXtChunk::encode(std::fstream& file) {
    try {
        uint32_t i =0;
        uint32_t dataLen = strlen(keyWord) + 1 + textString.size();
        write32(file, dataLen);
        file.write(CHUNK::typeMap[CHUNK::tEXt].c_str(), 4);
        auto ss = std::make_unique<uint8_t[]>(dataLen);
        for(;i<strlen(keyWord);i++){
            ss[i] = keyWord[i];
        }
        ss[i++] = 0;
        for(uint32_t j = 0;j< strlen(textString.c_str());j++,i++){
            ss[i] = textString[i];
        }
        file.write((char*)ss.get(), dataLen);
        crc.crc = createCRC(ss.get(), dataLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus zTXtChunk::decode(fstream& file, uint32_t length) {
    try {
        for (uint32_t i = 0; i < length; i++) {
            char ch;
            file.read((char*)&ch, sizeof(ch));
            if (ch == 0) {
                keyWord[i] = '\0';
                break;
            } else {
                keyWord[i] = ch;
            }
        }
        file.read((char*)&compressMethod, sizeof(compressMethod));
        uint32_t len = length - strlen(keyWord) - 2;
        compressDataStream.resize(len + 1);
        for (uint32_t i = 0; i < len; i++) {
            char ch;
            file.read((char*)&ch, sizeof(ch));
            compressDataStream[i] = ch;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus zTXtChunk::encode(std::fstream& file) {
    try {
        write32(file, strlen(keyWord) + 2 + compressDataStream.size());
        file.write(CHUNK::typeMap[CHUNK::zTXt].c_str(), 4);
        file.write(keyWord, strlen(keyWord));
        file << (uint8_t)0 << compressMethod;
        for (const auto& item : compressDataStream) {
            file << item;
        }
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus iTXtChunk::decode(fstream& file, uint32_t length) {
    try {
        uint32_t i = 0;
        for (; i < length; i++) {
            char ch;
            file.read((char*)&ch, sizeof(ch));
            if (ch == 0) {
                keyWord[i] = '\0';
                break;
            } else {
                keyWord[i] = ch;
            }
        }
        file.read((char*)&compressFlag, sizeof(compressFlag));
        file.read((char*)&compressMethod, sizeof(compressMethod));
        tagProcess(file, length, i);
        tKeyWordProcess(file, length, i);
        textProcess(file, length, i);
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus iTXtChunk::encode(std::fstream& file) {
    try {
        write32(file, strlen(keyWord) + 5 + languageTag.size() + translatedKeyWordLen + textLen);
        file.write(CHUNK::typeMap[CHUNK::iTXt].c_str(), 4);
        file.write(keyWord, strlen(keyWord));
        file << (uint8_t)0 << compressFlag << compressMethod;
        if (languageTag.size() > 0) file.write(languageTag.c_str(), strlen(languageTag.c_str()));
        file << (uint8_t)0;
        if (translatedKeyWordLen > 0) file.write((char*)translatedKeyWord.get(), translatedKeyWordLen);
        file << (uint8_t)0;
        if (textLen > 0) file.write((char*)text.get(), textLen);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

void iTXtChunk::tagProcess(fstream& file, uint32_t length, uint32_t& curPos) {
    queue<char> tagQueue;
    readQueue(file, length, curPos, tagQueue);
    languageTag.resize(tagQueue.size());
    for (uint32_t j = 0; j < tagQueue.size(); j++) {
        languageTag[j] = tagQueue.front();
        tagQueue.pop();
    }
}

void iTXtChunk::tKeyWordProcess(fstream& file, uint32_t length, uint32_t& curPos) {
    queue<uint8_t> q;
    readQueue(file, length, curPos, q);
    translatedKeyWord    = make_unique<uint8_t[]>(q.size());
    translatedKeyWordLen = q.size();
    for (uint32_t j = 0; j < q.size(); j++) {
        translatedKeyWord[j] = q.front();
        q.pop();
    }
}

void iTXtChunk::textProcess(fstream& file, uint32_t length, uint32_t& curPos) {
    uint32_t len = length - curPos - 1;
    text         = make_unique<uint8_t[]>(len);
    textLen      = len;
    for (uint32_t i = 0; i < len; i++) {
        uint8_t ch;
        file.read((char*)&ch, sizeof(ch));
        text[i] = ch;
    }
}

ImageStatus bKGDChunk::decode(fstream& file, uint32_t length) {
    try {
        switch (colourType) {
            case 0:
            case 4:
                file.read((char*)&_data.greyScale, sizeof(_data.greyScale));
                convertSmall16(_data.greyScale);
                break;
            case 2:
            case 6:
                file.read((char*)&_data.colour.red, 2);
                file.read((char*)&_data.colour.green, 2);
                file.read((char*)&_data.colour.blue, 2);
                convertSmall16(_data.colour.red);
                convertSmall16(_data.colour.green);
                convertSmall16(_data.colour.blue);
                break;
            case 3:
                file.read((char*)&_data.paletteIndex, sizeof(_data.paletteIndex));
                break;
            default:
                return ERROR_ENUM;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus bKGDChunk::encode(std::fstream& file) {
    try {
        uint32_t size = 2;
        if (colourType == 2 || colourType == 6) size = 6;
        else if (colourType == 3) size = 1;
        write32(file, size);
        file.write(CHUNK::typeMap[CHUNK::bKGD].c_str(), 4);
        switch (colourType) {
            case 0:
            case 4:
                write16(file, _data.greyScale);
                break;
            case 2:
            case 6: {
                write16(file, _data.colour.red);
                write16(file, _data.colour.green);
                write16(file, _data.colour.blue);
                break;
            }
            case 3:
                file << _data.paletteIndex;
                break;
            default:
                return ERROR_ENUM;
        }
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus hISTChunk::decode(fstream& file, uint32_t length) {
    if (length % 2 != 0) return ERROR_FILE_FORMAT;
    try {
        uint32_t len = length / 2;
        _data        = make_unique<uint16_t[]>(len);
        for (uint32_t i = 0; i < len; i++) {
            uint16_t integer;
            file.read((char*)&integer, sizeof(integer));
            convertSmall16(integer);
            _data[i] = integer;
        }
        _dataLen = len;
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus hISTChunk::encode(std::fstream& file) {
    try {
        write32(file, 2 * _dataLen);
        file.write(CHUNK::typeMap[CHUNK::hIST].c_str(), 4);
        for (uint32_t i = 0; i < _dataLen; i++) {
            write16(file, _data[i]);
        }
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus pHYsChunk::decode(fstream& file, uint32_t length) {
    try {
        file.read((char*)&pixelsPerUnitX, sizeof(pixelsPerUnitX));
        file.read((char*)&pixelsPerUnitY, sizeof(pixelsPerUnitY));
        file.read((char*)&unit, sizeof(unit));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(pixelsPerUnitX);
        convertSmall32(pixelsPerUnitY);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus pHYsChunk::encode(std::fstream& file) {
    try {
        write32(file, 9);
        file.write(CHUNK::typeMap[CHUNK::pHYs].c_str(), 4);
        write32(file, pixelsPerUnitX);
        write32(file, pixelsPerUnitY);
        file << unit;
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus sPLTChunk::decode(fstream& file, uint32_t length) {
    try {
        uint32_t pos = 0;
        nameProcess(file, length, pos);
        file.read((char*)&sampleDepth, sizeof(sampleDepth));
        uint32_t len  = length - paletteName.size() - sizeof(sampleDepth) - 1;
        uint32_t each = sampleDepth == 8 ? 6 : 10;
        if (len % each != 0 || (sampleDepth != 8 && sampleDepth != 16)) return ERROR_FILE_FORMAT;
        _entry   = make_unique<EntryData[]>(len / each);
        entryLen = len / each;
        for (uint32_t i = 0; i < len / each; i++) {
            EntryData entry {};
            file.read((char*)&entry.red, sampleDepth / 8);
            file.read((char*)&entry.green, sampleDepth / 8);
            file.read((char*)&entry.blue, sampleDepth / 8);
            file.read((char*)&entry.alpha, sampleDepth / 8);
            file.read((char*)&entry.frequency, 2);
            _entry[i] = entry;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus sPLTChunk::encode(std::fstream& file) {
    try {
        write32(file, paletteName.size() + 2 + entryLen * (sampleDepth == 8 ? 6 : 10));
        file.write(CHUNK::typeMap[CHUNK::sPLT].c_str(), 4);
        if (!paletteName.empty()) file.write(paletteName.c_str(), paletteName.size());
        file << (uint8_t)0 << sampleDepth;
        for (uint32_t i = 0; i < entryLen; i++) {
            auto temp = _entry[i];
            if (sampleDepth == 8) {
                file << (uint8_t)temp.red << (uint8_t)temp.green << (uint8_t)temp.blue << (uint8_t)temp.alpha;
            } else if (sampleDepth == 16) {
                write16(file, temp.red);
                write16(file, temp.green);
                write16(file, temp.blue);
                write16(file, temp.alpha);
            }
            write16(file, temp.frequency);
        }
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

void sPLTChunk::nameProcess(fstream& file, uint32_t length, uint32_t& curPos) {
    queue<char> nameQueue;
    readQueue(file, length, curPos, nameQueue);
    paletteName.resize(nameQueue.size());
    for (uint32_t j = 0; j < nameQueue.size(); j++) {
        paletteName[j] = nameQueue.front();
        nameQueue.pop();
    }
}

ImageStatus eXIfChunk::decode(fstream& file, uint32_t length) {
    try {
        file.seekg(length, ios::cur);
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus eXIfChunk::encode(std::fstream& file) {
    try {
        write32(file, 0);
        file.write(CHUNK::typeMap[CHUNK::eXIf].c_str(), 4);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus tIMEChunk::decode(fstream& file, uint32_t length) {
    try {
        file.read((char*)&year, sizeof(year));
        file.read((char*)&month, sizeof(month));
        file.read((char*)&day, sizeof(day));
        file.read((char*)&hour, sizeof(hour));
        file.read((char*)&minute, sizeof(minute));
        file.read((char*)&second, sizeof(second));
        file.read((char*)&crc, sizeof(crc));
        convertSmall16(year);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus tIMEChunk::encode(std::fstream& file) {
    try {
        write32(file, 7);
        file.write(CHUNK::typeMap[CHUNK::tIME].c_str(), 4);
        write16(file, year);
        file << month << day << hour << minute << second;
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus acTLChunk::decode(fstream& file, uint32_t length) {
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(_data.numFrames);
        convertSmall32(_data.numPlays);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus acTLChunk::encode(std::fstream& file) {
    try {
        write32(file, 8);
        file.write(CHUNK::typeMap[CHUNK::acTL].c_str(), 4);
        write32(file, _data.numFrames);
        write32(file, _data.numPlays);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus fcTLChunk::decode(fstream& file, uint32_t length) {
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&disposeOp, sizeof(disposeOp));
        file.read((char*)&blendOp, sizeof(blendOp));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(_data.sequenceNumber);
        convertSmall32(_data.width);
        convertSmall32(_data.height);
        convertSmall32(_data.offsetX);
        convertSmall32(_data.offsetY);
        convertSmall16(_data.delayNum);
        convertSmall16(_data.delayDen);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus fcTLChunk::encode(std::fstream& file) {
    try {
        write32(file, 26);
        file.write(CHUNK::typeMap[CHUNK::fcTL].c_str(), 4);
        write32(file, _data.sequenceNumber);
        write32(file, _data.width);
        write32(file, _data.height);
        write32(file, _data.offsetX);
        write32(file, _data.offsetY);
        write16(file, _data.delayNum);
        write16(file, _data.delayDen);
        file << disposeOp << blendOp;
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus fdATChunk::decode(fstream& file, uint32_t length) {
    try {
        file.read((char*)&sequenceNumber, sizeof(sequenceNumber));
        uint32_t len = length - sizeof(sequenceNumber);
        frameData    = make_unique<uint8_t[]>(len);
        dataLen      = len;
        for (uint32_t i = 0; i < len; i++) {
            frameData[i] = file.get();
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(sequenceNumber);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus fdATChunk::encode(std::fstream& file) {
    try {
        write32(file, 4 + dataLen);
        file.write(CHUNK::typeMap[CHUNK::fdAT].c_str(), 4);
        write32(file, sequenceNumber);
        for (uint32_t i = 0; i < dataLen; i++) {
            file << frameData[i];
        }
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus IDATChunk::decode(fstream& file, uint32_t length, std::vector<std::pair<long, long>>& IDATMap) {
    try {
#ifdef _IDAT_DEBUG_
        fstream hexFile("E:/code/mySource/log/hexFile.txt", ios::out | ios::app);
        cout << hex << file.tellg() << endl;
#endif
        IDATMap.emplace_back(file.tellg(), length);
#ifdef _IDAT_DEBUG_
        for (uint32_t i = 0; i < length; i++) {
            if (i % 16 == 0) hexFile << endl;
            hexFile.width(2);
            hexFile.fill('0');
            hexFile << hex << (int)_data[i] << " ";
        }
#else
        file.seekg(length, std::ios::cur);
#endif
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus IDATChunk::encode(std::fstream& file, unique_ptr<uint8_t[]>& data, long dataLen) {
    try {
        long totalLen = dataLen;
        do {
            long count = 0l;
            if (dataLen >= IDAT_MAX_LEN) count = IDAT_MAX_LEN;
            else count = dataLen;
            write32(file, count);
            file.write(CHUNK::typeMap[CHUNK::IDAT].c_str(), 4);
            file.write((char*)(data.get() + totalLen - dataLen), count);
            write32(file, crc.crc);
            dataLen -= count;
        } while (dataLen > 0);
        // compress data
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus IENDChunk::decode(fstream& file, uint32_t length) {
    try {
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus IENDChunk::encode(std::fstream& file) {
    try {
        write32(file, 0);
        file.write(CHUNK::typeMap[CHUNK::IEND].c_str(), 4);
        write32(file, crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus PNGData::read(const char* filePath) {
    CHECK_NULL_RETURN(filePath, ERROR_NULLPTR)
    _filePath = string(filePath);
    fstream file(filePath, ios::in | ios::binary);
    if (file.fail()) return ERROR_FILE_OPERATOR;
    try {
        file.read((char*)&_formatFlag, sizeof(_formatFlag));
        if (!checkFormat()) return ERROR_FILE_FORMAT;
        ChunkHead head {};
        do {
            head.reset();
            file.read((char*)&head, sizeof(head));
            convertSmall32(head.length);
            convertSmall32(head.chunkType);
#ifdef _DEBUG_
            TEMP_LOG("chunkType:%s, length:%d\n", CHUNK::typeMap[head.chunkType].c_str(), head.length)
#endif
            if (decodeProcess(file, head) != SUCCESS) return ERROR_FILE_DECODE;
        } while (head.chunkType != CHUNK::IEND && !file.eof());
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus PNGData::decodeProcess(fstream& file, const ChunkHead& head) {
    ImageStatus status = ERROR_UNKNOWN;
    try {
        switch (head.chunkType) {
            case CHUNK::IHDR: {
                status                 = _IHDR.decode(file, head.length);
                _minCompressDataLength = _IHDR.minCompressDataLength();
                break;
            }
            case CHUNK::IDAT: {
                IDATChunk iDATChunk;
                status    = iDATChunk.decode(file, head.length, _IDATMap);
                _iDATLen += static_cast<long>(head.length);
                break;
            }
            case CHUNK::IEND: {
                status = _IEND.decode(file, head.length);
                if (status != SUCCESS) break;
                status = decodeIDATDataProcess(file);
                break;
            }
            default:
                return ancillaryChunkFactory(file, head);
        }
    } catch (...) { return ERROR_UNKNOWN; }
    return status;
}

ImageStatus PNGData::ancillaryChunkFactory(fstream& file, const ChunkHead& head) {
    ANCILLARY_CHUNK_FACTORY(PLTE)
    ANCILLARY_CHUNK_FACTORY_FUNC(tRNS, ((tRNSChunk*)chunk)->colourType = _IHDR._data.colorType)
    ANCILLARY_CHUNK_FACTORY(cHRM)
    ANCILLARY_CHUNK_FACTORY(gAMA)
    ANCILLARY_CHUNK_FACTORY(iCCP)
    ANCILLARY_CHUNK_FACTORY_FUNC(sBIT, ((sBITChunk*)chunk)->colourType = _IHDR._data.colorType)
    ANCILLARY_CHUNK_FACTORY(sRGB)
    ANCILLARY_CHUNK_FACTORY(cICP)
    ANCILLARY_CHUNK_FACTORY(mDCv)
    ANCILLARY_CHUNK_FACTORY(cLLi)
    ANCILLARY_CHUNK_FACTORY(tEXt)
    ANCILLARY_CHUNK_FACTORY(zTXt)
    ANCILLARY_CHUNK_FACTORY(iTXt)
    ANCILLARY_CHUNK_FACTORY_FUNC(bKGD, ((bKGDChunk*)chunk)->colourType = _IHDR._data.colorType)
    ANCILLARY_CHUNK_FACTORY(hIST)
    ANCILLARY_CHUNK_FACTORY(pHYs)
    ANCILLARY_CHUNK_FACTORY(sPLT)
    ANCILLARY_CHUNK_FACTORY(eXIf)
    ANCILLARY_CHUNK_FACTORY(tIME)
    ANCILLARY_CHUNK_FACTORY(acTL)
    ANCILLARY_CHUNK_FACTORY(fcTL)
    ANCILLARY_CHUNK_FACTORY(fdAT)
    return ERROR_STRUCT_NOT_DEFINE;
}

ImageStatus PNGData::decodeIDATDataProcess(fstream& file) {
    ImageStatus result = SUCCESS;
    if ((result = getIDATData(file)) != SUCCESS) return result;
    if ((result = uncompress_UndoFilter()) != SUCCESS) return result;
    return result;
}

ImageStatus PNGData::getIDATData(fstream& file) {
    try {
        _data.reset();
        _data    = std::make_unique<uint8_t[]>(_iDATLen);
        long cur = 0;
        for (const auto& item : _IDATMap) {
            file.seekg(item.first, std::ios::beg);
            file.read((char*)(_data.get() + cur), item.second);
            cur += item.second;
        }
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus PNGData::uncompress_UndoFilter() {
    try {
        // uncompress
        uLong len = (_minCompressDataLength * _IHDR.width() + 1) * _IHDR.height();
        //        auto uncompressData = std::make_unique<uint8_t[]>(len);
        //        int err             = compress(uncompressData.get(), &len, _data.get(), _iDATLen);
        //        if (err != Z_OK) return ERROR_UNCOMPRESS;
        auto uncompressData = std::make_unique<uint8_t[]>(len);
        z_stream stream;
        stream.zalloc    = Z_NULL;
        stream.zfree     = Z_NULL;
        stream.opaque    = Z_NULL;
        stream.avail_in  = _iDATLen;
        stream.next_in   = _data.get();
        stream.avail_out = len;
        stream.next_out  = uncompressData.get();
        inflateInit(&stream);
        inflate(&stream, Z_NO_FLUSH);
        inflateEnd(&stream);
        _data.reset();
        // undo filter
        switch (_IHDR.filterMethod()) {
            case 0:
                if (!undoFilter0(uncompressData, len)) return ERROR_UNDO_FILTER;
                return SUCCESS;
            case 3:
            case 4:
                return ERROR_METHOD_UNDEFINE;
        }
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

bool PNGData::undoFilter0(unique_ptr<uint8_t[]>& uncompressData, unsigned long length) {
    try {
        uint32_t len = _minCompressDataLength * _IHDR.width() + 1;
        for (uint32_t i = 0; i < length; i += len) {
            switch (uncompressData[i]) {
                case CHUNK::NONE:
                    continue;
                case CHUNK::SUB:
                    png_undo_filter_sub(uncompressData, i + 1, len - 1);
                    break;
                case CHUNK::UP:
                    png_undo_filter_up(uncompressData, i + 1, len - 1);
                    break;
                case CHUNK::AVERAGE:
                    png_undo_filter_average(uncompressData, i + 1, len - 1);
                    break;
                case CHUNK::PAETH:
                    png_undo_filter_paeth(uncompressData, i + 1, len - 1);
                    break;
                default:
                    return false;
            }
        }
        data2Matrix(uncompressData);
    } catch (...) { return false; }
    return true;
}

bool PNGData::filter0(unique_ptr<uint8_t[]>& originData, unsigned long originLen) {
    try {
        uint32_t rowLen = _minCompressDataLength * _IHDR.width() + 1;
        for (uint32_t i = 0; i < _IHDR.height(); i++) {
            switch (originData[i * rowLen]) {
                case CHUNK::NONE:
                    continue;
                case CHUNK::SUB:
                    png_undo_filter_sub(originData, i * rowLen + 1, rowLen - 1);
                    break;
                case CHUNK::UP:
                    png_undo_filter_up(originData, i * rowLen + 1, rowLen - 1);
                    break;
                case CHUNK::AVERAGE:
                    png_undo_filter_average(originData, i * rowLen + 1, rowLen - 1);
                    break;
                case CHUNK::PAETH:
                    png_undo_filter_paeth(originData, i * rowLen + 1, rowLen - 1);
                    break;
                default:
                    return false;
            }
        }
    } catch (...) { return false; }
    return true;
}

void PNGData::png_undo_filter_sub(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) const {
    for (uint32_t i = curPos + _minCompressDataLength; i < curPos + length; i += 1) {
        uncompressData[i] += uncompressData[i - _minCompressDataLength];
    }
}

void PNGData::png_filter_sub(unique_ptr<uint8_t[]>& originData, uint32_t curPos, uint32_t length) {
    for (uint32_t i = curPos + _minCompressDataLength; i < curPos + length; i++) {
        originData[i] -= originData[i - _minCompressDataLength];
    }
}

void PNGData::png_undo_filter_up(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) {
    if (curPos < length + 1) return;
    for (uint32_t i = curPos, j = curPos - length - 1; i < curPos + length; i++, j++) {
        uncompressData[i] += uncompressData[j];
    }
}

void PNGData::png_filter_up(unique_ptr<uint8_t[]>& originData, uint32_t curPos, uint32_t length) {
    if (curPos < length + 1) return;
    for (uint32_t i = curPos, j = curPos - length - 1; i < curPos + length; i++, j++) {
        originData[i] -= originData[j];
    }
}

void PNGData::png_undo_filter_average(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) const {
    if (curPos < length + 1) return;
    for (uint32_t i = curPos; i < curPos + length; i++) {
        uint8_t a          = (i - _minCompressDataLength) < curPos ? 0 : uncompressData[i - _minCompressDataLength];
        uint8_t b          = uncompressData[i - length - 1];
        uncompressData[i] += ((int32_t)a + (int32_t)b) / 2;
    }
}

void PNGData::png_filter_average(unique_ptr<uint8_t[]>& originData, uint32_t curPos, uint32_t length) {
    if (curPos < length + 1) return;
    for (uint32_t i = curPos; i < curPos + length; i++) {
        uint8_t a      = (i - _minCompressDataLength) < curPos ? 0 : originData[i - _minCompressDataLength];
        uint8_t b      = originData[i - length - 1];
        originData[i] -= ((int32_t)a + (int32_t)b) / 2;
    }
}

void PNGData::png_undo_filter_paeth(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) {
    if (curPos < length + 1) return;
    for (uint32_t i = curPos; i < curPos + length; i++) {
        uint8_t a = (i - _minCompressDataLength) < curPos ? 0 : uncompressData[i - _minCompressDataLength];
        uint8_t b = uncompressData[i - length - 1];
        uint8_t c = (i - _minCompressDataLength) < curPos ? 0 : uncompressData[i - _minCompressDataLength - length - 1];
        uncompressData[i] += PaethPredictor(a, b, c);
    }
}

void PNGData::png_filter_paeth(unique_ptr<uint8_t[]>& originData, uint32_t curPos, uint32_t length) {
    if (curPos < length + 1) return;
    for (uint32_t i = curPos; i < curPos + length; i++) {
        uint8_t a = (i - _minCompressDataLength) < curPos ? 0 : originData[i - _minCompressDataLength];
        uint8_t b = originData[i - length - 1];
        uint8_t c = (i - _minCompressDataLength) < curPos ? 0 : originData[i - _minCompressDataLength - length - 1];
        originData[i] -= PaethPredictor(a, b, c);
    }
}

void PNGData::data2Matrix(unique_ptr<uint8_t[]>& uncompressData) {
    _rgb           = new Matrix<RGB>(_IHDR.height(), _IHDR.width());
    uint8_t curBit = 0;
    // read all bitDepth
    auto convertRGBA = [&](uint8_t*& pos, uint8_t bitDepth, myUtil::CHUNK::ColorType type) {
        uint32_t count = CHUNK::ColorTypeBitMap[type];
        if (bitDepth >= 8) {
            uint16_t temp[4] {0};
            int32_t bit = bitDepth / 8;
            for (int32_t i = 0; i < count; i++) {
                uint16_t value = 0;
                for (int32_t j = 0; j < bit; j++) {
                    value = (value << 8) | *(pos + i * bit + j);
                }
                temp[i] = value;
            }
            pos += _minCompressDataLength;
            if (type == CHUNK::GREY_SCALE) return RGB {temp[0], temp[0], temp[0], 0};
            else if (type == CHUNK::RGB) return RGB {temp[0], temp[1], temp[2], 0};
            else if (type == CHUNK::INDEXED_COLOUR) {
                auto chunk   = dynamic_cast<PLTEChunk*>(_ancillaryChunk[CHUNK::PLTE]);
                auto palette = chunk->palette()[temp[0]];
                return RGB {palette.rgbRed, palette.rgbGreen, palette.rgbBlue, 0};
            } else if (type == CHUNK::GREY_ALPHA) return RGB {temp[0], temp[0], temp[0], temp[1]};
            else if (type == CHUNK::RGBA) return RGB {temp[0], temp[1], temp[2], temp[3]};
        } else {
            // 暂未实现位深度为1，2，4的GREY_SCALE和INDEXED_COLOUR
            uint16_t temp = ((*pos) << curBit) >> (8 - bitDepth);
            if ((curBit + bitDepth) >= 8) pos++;
            curBit = (curBit + bitDepth) % 8;
            if (type == CHUNK::GREY_SCALE) return RGB {temp, temp, temp, 0};
            else if (type == CHUNK::INDEXED_COLOUR) {
                auto chunk   = dynamic_cast<PLTEChunk*>(_ancillaryChunk[CHUNK::PLTE]);
                auto palette = chunk->palette()[temp];
                return RGB {palette.rgbRed, palette.rgbGreen, palette.rgbBlue, 0};
            }
        }
        return RGB {};
    };
    auto* pos = uncompressData.get() + 1;
    for (int32_t i = 0; i < _IHDR.height(); i++) {
        for (int32_t j = 0; j < _IHDR.width(); j++) {
            _rgb->setValue(i, j, convertRGBA(pos, _IHDR.bitDepth(), static_cast<CHUNK::ColorType>(_IHDR.colorType())));
        }
        pos += 1;
    }
}

ImageStatus PNGData::write(const char* filePath) {
    CHECK_NULL_RETURN(filePath, ERROR_NULLPTR)
    try {
        fstream file(filePath, std::ios::out | std::ios::binary);
        if (file.fail()) return ERROR_FILE_OPERATOR;
        file.write((char*)&PNG_FLAG, sizeof(PNG_FLAG));
        _minCompressDataLength = _IHDR.minCompressDataLength();
        _IHDR.encode(file);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

void PNGData::matrix2Data(unique_ptr<uint8_t[]>& originData) {}
NAME_SPACE_END()
