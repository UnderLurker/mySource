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
                if (length != sizeof(ChunkColour)) return ERROR_FILE_FORMAT;
                file.read((char*)&_data.colourType2, sizeof(ChunkColour));
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

ImageStatus sBITChunk::decode(fstream& file, uint32_t length) {
    try {
        switch (colourType) {
            case CHUNK::GREY_SCALE: {
                file.read((char*)&_data.colourType0, sizeof(_data.colourType0));
                break;
            }
            case CHUNK::RGB:
            case CHUNK::INDEXED_COLOUR: {
                file.read((char*)&_data.colourType23, sizeof(_data.colourType23));
                break;
            }
            case CHUNK::GREY_ALPHA: {
                file.read((char*)&_data.colourType4, sizeof(_data.colourType4));
                break;
            }
            case CHUNK::RGBA: {
                file.read((char*)&_data.colourType6, sizeof(_data.colourType6));
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

ImageStatus sRGBChunk::decode(fstream& file, uint32_t length) {
    if (length != sizeof(_data)) return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
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
    translatedKeyWord = make_unique<uint8_t[]>(q.size());
    for (uint32_t j = 0; j < q.size(); j++) {
        translatedKeyWord[j] = q.front();
        q.pop();
    }
}

void iTXtChunk::textProcess(fstream& file, uint32_t length, uint32_t& curPos) {
    uint32_t len = length - curPos - 1;
    text         = make_unique<uint8_t[]>(len);
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
                file.read((char*)&_data.colour, sizeof(_data.colour));
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
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
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

ImageStatus sPLTChunk::decode(fstream& file, uint32_t length) {
    try {
        uint32_t pos = 0;
        nameProcess(file, length, pos);
        file.read((char*)&sampleDepth, sizeof(sampleDepth));
        uint32_t len = length - paletteName.size() - sizeof(sampleDepth) - 1;
        if (len % 3 != 0 || (sampleDepth != 8 && sampleDepth != 16)) return ERROR_FILE_FORMAT;
        _entry = make_unique<PaletteEntry[]>(len / 3);
        for (uint32_t i = 0; i < len / 3; i++) {
            PaletteEntry entry {};
            if (sampleDepth == 8) {
                paletteEntryProcess(file, entry);
                continue;
            }
            file.read((char*)&entry, sizeof(entry));
            _entry[i] = entry;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
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

void sPLTChunk::paletteEntryProcess(fstream& file, PaletteEntry& entry) {
    uint8_t color;
    file.read((char*)&color, sizeof(uint8_t));
    entry.red = color;
    file.read((char*)&color, sizeof(uint8_t));
    entry.green = color;
    file.read((char*)&color, sizeof(uint8_t));
    entry.blue = color;
    file.read((char*)&color, sizeof(uint8_t));
    entry.alpha = color;
}

ImageStatus eXIfChunk::decode(fstream& file, uint32_t length) {
    try {
        file.seekg(length, ios::cur);
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
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

ImageStatus fdATChunk::decode(fstream& file, uint32_t length) {
    try {
        file.read((char*)&sequenceNumber, sizeof(sequenceNumber));
        uint32_t len = length - sizeof(sequenceNumber);
        frameData    = make_unique<uint8_t[]>(len);
        for (uint32_t i = 0; i < len; i++) {
            frameData[i] = file.get();
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(sequenceNumber);
        convertSmall32(crc.crc);
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

ImageStatus IDATChunk::decode(fstream& file, uint32_t length, std::vector<std::pair<long, long>>& IDATMap) {
    try {
#ifdef _IDAT_DEBUG_
        fstream hexFile("E:/code/mySource/log/hexFile.txt", ios::out | ios::app);
#endif
        cout << hex << file.tellg() << endl;
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

ImageStatus IENDChunk::decode(fstream& file, uint32_t length) {
    try {
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
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
        file.seekg(4, ios::cur); // formatFlag's CRC
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
                _width                 = _IHDR.width();
                _height                = _IHDR.height();
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
    ANCILLARY_CHUNK_FACTORY(sBIT)
    ANCILLARY_CHUNK_FACTORY(sRGB)
    ANCILLARY_CHUNK_FACTORY(cICP)
    ANCILLARY_CHUNK_FACTORY(mDCv)
    ANCILLARY_CHUNK_FACTORY(cLLi)
    ANCILLARY_CHUNK_FACTORY(tEXt)
    ANCILLARY_CHUNK_FACTORY(zTXt)
    ANCILLARY_CHUNK_FACTORY(iTXt)
    ANCILLARY_CHUNK_FACTORY(bKGD)
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

void PNGData::png_undo_filter_sub(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) const {
    for (int32_t i = curPos + _minCompressDataLength; i < curPos + length; i += _minCompressDataLength) {
        int32_t k = i;
        for (int32_t j = i - _minCompressDataLength; j < i; j++, k++) {
            uncompressData[k] += uncompressData[j];
        }
    }
}

void PNGData::png_undo_filter_up(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) {
    if (curPos < length + 1) return;
    for (int32_t i = curPos, j = curPos - length - 1; i < curPos + length; i++, j++) {
        uncompressData[i] += uncompressData[j];
    }
}

void PNGData::png_undo_filter_average(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) const {
    if (curPos < length + 1) return;
    for (int32_t i = curPos; i < curPos + length; i++) {
        uint8_t a          = (i - _minCompressDataLength) < curPos ? 0 : uncompressData[i - _minCompressDataLength];
        uint8_t b          = uncompressData[i - length - 1];
        uncompressData[i] += ((int32_t)a + (int32_t)b) / 2;
    }
}

void PNGData::png_undo_filter_paeth(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) {
    auto PaethPredictor = [=](uint8_t a, uint8_t b, uint8_t c) {
        int32_t p  = a + b - c;
        int32_t pa = abs(p - a);
        int32_t pb = abs(p - b);
        int32_t pc = abs(p - c);
        if (pa <= pb && pa <= pc) return a;
        else if (pb <= pc) return b;
        else return c;
    };
    if (curPos < length + 1) return;
    for (int32_t i = curPos; i < curPos + length; i++) {
        uint8_t a = (i - _minCompressDataLength) < curPos ? 0 : uncompressData[i - _minCompressDataLength];
        uint8_t b = uncompressData[i - length - 1];
        uint8_t c = (i - _minCompressDataLength) < curPos ? 0 : uncompressData[i - _minCompressDataLength - length - 1];
        uncompressData[i] += PaethPredictor(a, b, c);
    }
}

void PNGData::data2Matrix(unique_ptr<uint8_t[]>& uncompressData) {
    _rgb             = new Matrix<RGB>(_IHDR.height(), _IHDR.width());
    auto convertRGBA = [&](uint8_t* pos, uint8_t bitDepth, myUtil::CHUNK::ColorType type) {
        uint32_t count = CHUNK::ColorTypeBitMap[type];
        RGB res {};
        if (bitDepth >= 8) {
            uint8_t temp[4] {0};
            int32_t bit = bitDepth / 8;
            for (int32_t i = 0; i < count; i++) {
                int32_t value = 0;
                for (int32_t j = 0; j < bit; j++) {
                    value = (value << 8) | *(pos + i * bit + j);
                }
                temp[i] = value;
            }
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
        }
        return res;
    };
    auto* pos = uncompressData.get() + 1;
    for (int32_t i = 0; i < _IHDR.height(); i++) {
        for (int32_t j = 0; j < _IHDR.width(); j++) {
            _rgb->setValue(i, j, convertRGBA(pos, _IHDR.bitDepth(), static_cast<CHUNK::ColorType>(_IHDR.colorType())));
            pos += _minCompressDataLength;
        }
        pos += 1;
    }
}
NAME_SPACE_END()
