#include "png.h"

#include <cstdint>
#include <fstream>
#include <windef.h>

#include "ImageUtil.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

void convertSmall16(uint16_t& value)
{
    uint16_t value1 = value & 0x00ff;
    uint16_t value2 = value & 0xff00;
    value = (value1 << 8) | (value2 >> 8);
}

void convertSmall32(uint32_t& value)
{
    uint32_t value1 = value & 0x000000ff;
    uint32_t value2 = value & 0x0000ff00;
    uint32_t value3 = value & 0x00ff0000;
    uint32_t value4 = value & 0xff000000;
    value = (value1 << 24) | (value2 << 8) | (value3 >> 8) | (value4 >> 24);
}

ImageStatus IHDRChunk::decode(fstream& file, uint32_t length)
{
    if (length <= 0)
        return ERROR_LIMIT;
    try {
        file.read((char*)&_data, length);
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(_data.width);
        convertSmall32(_data.height);
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus PLETChunk::decode(fstream& file, uint32_t length)
{
    if (length % 3 == 0)
        return ERROR_FILE_FORMAT;
    try {
        uint32_t len = length / 3;
        _data.resize(len);
        for (uint32_t i = 0; i < len; i++) {
            PLETData data {};
            file.read((char*)&data, sizeof(PLETData));
            _data.at(i) = data;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus tRNSChunk::decode(fstream& file, uint32_t length)
{
    try {
        switch (colourType) {
            case 0: {
                if (length != 2)
                    return ERROR_FILE_FORMAT;
                file.read((char*)&_data.colourType0, sizeof(_data.colourType0));
                convertSmall16(_data.colourType0);
                break;
            }
            case 2: {
                if (length != sizeof(ChunkColour))
                    return ERROR_FILE_FORMAT;
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus cHRMChunk::decode(fstream& file, uint32_t length)
{
    if (length != sizeof(cHRMData))
        return ERROR_FILE_FORMAT;
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus gAMAChunk::decode(fstream& file, uint32_t length)
{
    if (length != sizeof(_data))
        return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(_data);
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus iCCPChunk::decode(fstream& file, uint32_t length)
{
    try {
        uint8_t ch = 1;
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus sBITChunk::decode(fstream& file, uint32_t length)
{
    try {
        switch (colourType) {
            case 0: {
                file.read((char*)&_data.colourType0, sizeof(_data.colourType0));
                break;
            }
            case 2:
            case 3: {
                file.read((char*)&_data.colourType23, sizeof(_data.colourType23));
                break;
            }
            case 4: {
                file.read((char*)&_data.colourType4, sizeof(_data.colourType4));
                break;
            }
            case 6: {
                file.read((char*)&_data.colourType6, sizeof(_data.colourType6));
                break;
            }
            default:
                break;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus sRGBChunk::decode(fstream& file, uint32_t length)
{
    if (length != sizeof(_data))
        return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus cICPChunk::decode(fstream& file, uint32_t length)
{
    if (length != sizeof(_data))
        return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus mDCvChunk::decode(fstream& file, uint32_t length)
{
    if (length != sizeof(_data))
        return ERROR_FILE_FORMAT;
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

vector<mDCvChunk::Chromaticity> mDCvChunk::chromaticity()
{
    return {
        { _data.chromaticityX1 * 0.00002f, _data.chromaticityY1 * 0.00002f },
        { _data.chromaticityX2 * 0.00002f, _data.chromaticityY2 * 0.00002f },
        { _data.chromaticityX3 * 0.00002f, _data.chromaticityY3 * 0.00002f },
    };
}

mDCvChunk::Chromaticity mDCvChunk::whitePoint()
{
    return { _data.whitePointX * 0.00002f, _data.whitePointY * 0.00002f };
}

float mDCvChunk::maxLum()
{
    return _data.maxLum * 0.0001;
}

float mDCvChunk::minLum()
{
    return _data.minLum * 0.0001;
}

ImageStatus cLLiChunk::decode(fstream& file, uint32_t length)
{
    if (length != sizeof(uint32_t) * 2)
        return ERROR_FILE_FORMAT;
    try {
        file.read((char*)&MaxCLL, sizeof(MaxCLL));
        file.read((char*)&MaxFALL, sizeof(MaxFALL));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(MaxCLL);
        convertSmall32(MaxFALL);
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus tEXtChunk::decode(fstream& file, uint32_t length)
{
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus zTXtChunk::decode(fstream& file, uint32_t length)
{
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus iTXtChunk::decode(fstream& file, uint32_t length)
{
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

void iTXtChunk::tagProcess(fstream& file, uint32_t length, uint32_t& curPos)
{
    queue<char> tagQueue;
    readQueue(file, length, curPos, tagQueue);
    languageTag.resize(tagQueue.size());
    for (uint32_t j = 0; j < tagQueue.size(); j++) {
        languageTag[j] = tagQueue.front();
        tagQueue.pop();
    }
}

void iTXtChunk::tKeyWordProcess(fstream& file, uint32_t length, uint32_t& curPos)
{
    queue<uint8_t> q;
    readQueue(file, length, curPos, q);
    translatedKeyWord = make_unique<uint8_t[]>(q.size());
    for (uint32_t j = 0; j < q.size(); j++) {
        translatedKeyWord[j] = q.front();
        q.pop();
    }
}

void iTXtChunk::textProcess(fstream& file, uint32_t length, uint32_t& curPos)
{
    uint32_t len = length - curPos - 1;
    text = make_unique<uint8_t[]>(len);
    for (uint32_t i = 0; i < len; i++) {
        uint8_t ch;
        file.read((char*)&ch, sizeof(ch));
        text[i] = ch;
    }
}

ImageStatus bKGDChunk::decode(fstream& file, uint32_t length)
{
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus hISTChunk::decode(fstream& file, uint32_t length)
{
    if (length % 2 != 0)
        return ERROR_FILE_FORMAT;
    try {
        uint32_t len = length / 2;
        _data = make_unique<uint16_t[]>(len);
        for (uint32_t i = 0; i < len; i++) {
            uint16_t integer;
            file.read((char*)&integer, sizeof(integer));
            convertSmall16(integer);
            _data[i] = integer;
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus pHYsChunk::decode(fstream& file, uint32_t length)
{
    try {
        file.read((char*)&pixelsPerUnitX, sizeof(pixelsPerUnitX));
        file.read((char*)&pixelsPerUnitY, sizeof(pixelsPerUnitY));
        file.read((char*)&unit, sizeof(unit));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(pixelsPerUnitX);
        convertSmall32(pixelsPerUnitY);
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus sPLTChunk::decode(fstream& file, uint32_t length)
{
    try {
        uint32_t pos = 0;
        nameProcess(file, length, pos);
        file.read((char*)&sampleDepth, sizeof(sampleDepth));
        uint32_t len = length - paletteName.size() - sizeof(sampleDepth) - 1;
        if (len % 3 != 0 || (sampleDepth != 8 && sampleDepth != 16))
            return ERROR_FILE_FORMAT;
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

void sPLTChunk::nameProcess(fstream& file, uint32_t length, uint32_t& curPos)
{
    queue<char> nameQueue;
    readQueue(file, length, curPos, nameQueue);
    paletteName.resize(nameQueue.size());
    for (uint32_t j = 0; j < nameQueue.size(); j++) {
        paletteName[j] = nameQueue.front();
        nameQueue.pop();
    }
}

void sPLTChunk::paletteEntryProcess(fstream& file, PaletteEntry& entry)
{
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

ImageStatus eXIfChunk::decode(fstream& file, uint32_t length)
{
    try {
        file.seekg(length, ios::cur);
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus tIMEChunk::decode(fstream& file, uint32_t length)
{
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus acTLChunk::decode(fstream& file, uint32_t length)
{
    try {
        file.read((char*)&_data, sizeof(_data));
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(_data.numFrames);
        convertSmall32(_data.numPlays);
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus fcTLChunk::decode(fstream& file, uint32_t length)
{
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
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus fdATChunk::decode(fstream& file, uint32_t length)
{
    try {
        file.read((char*)&sequenceNumber, sizeof(sequenceNumber));
        uint32_t len = length - sizeof(sequenceNumber);
        frameData = make_unique<uint8_t[]>(len);
        for (uint32_t i = 0; i < len; i++) {
            frameData[i] = file.get();
        }
        file.read((char*)&crc, sizeof(crc));
        convertSmall32(sequenceNumber);
        convertSmall32(crc.crc);
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus PNGData::read(const char* filePath)
{
    CHECK_NULL_RETURN(filePath, ERROR_NULLPTR)
    fstream file(filePath, ios::in | ios::binary);
    if (file.fail())
        return ERROR_FILE_OPERATOR;
    try {
        file.seekg(8, ios::beg);
        ChunkHead head {};
        do {
            head.reset();
            file.read((char*)&head, sizeof(head));
            convertSmall32(head.length);
            convertSmall32(head.chunkType);
            if (decodeProcess(file, head) != SUCCESS)
                return ERROR_FILE_DECODE;
        } while (head.chunkType != IEND && !file.eof());
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

ImageStatus PNGData::decodeProcess(fstream& file, const ChunkHead& head)
{
    ImageStatus status = ERROR_UNKNOWN;
    try {
        switch (head.chunkType) {
            case IHDR:
                status = _IHDR.decode(file, head.length);
                _width = _IHDR.width();
                _height = _IHDR.height();
                break;
            case PLTE:
                status = _PLTE.decode(file, head.length);
                break;
            case tRNS:
                status = _tRNS.decode(file, head.length);
                break;
            case cHRM:
                status = _cHRM.decode(file, head.length);
                break;
            case gAMA:
                status = _gAMA.decode(file, head.length);
                break;
            case iCCP:
                status = _iCCP.decode(file, head.length);
                break;
            case sBIT:
                status = _sBIT.decode(file, head.length);
                break;
            case sRGB:
                status = _sRGB.decode(file, head.length);
                break;
            case cICP:
                status = _cICP.decode(file, head.length);
                break;
            case mDCv:
                status = _mDCv.decode(file, head.length);
                break;
            case cLLi:
                status = _cLLi.decode(file, head.length);
                break;
            case tEXt:
                status = _tEXt.decode(file, head.length);
                break;
            case zTXt:
                status = _zTXt.decode(file, head.length);
                break;
            case iTXt:
                status = _iTXt.decode(file, head.length);
                break;
            case bKGD:
                status = _bKGD.decode(file, head.length);
                break;
            case hIST:
                status = _hIST.decode(file, head.length);
                break;
            case pHYs:
                status = _pHYs.decode(file, head.length);
                break;
            case sPLT:
                status = _sPLT.decode(file, head.length);
                break;
            case eXIf:
                status = _eXIf.decode(file, head.length);
                break;
            case tIME:
                status = _tIME.decode(file, head.length);
                break;
            case acTL:
                status = _acTL.decode(file, head.length);
                break;
            case fcTL:
                status = _fcTL.decode(file, head.length);
                break;
            case fdAT:
                status = _fdAT.decode(file, head.length);
                break;
            case IEND:
            case IDAT:
            default:
                file.seekg(head.length + 4, ios::cur);
                auto a = file.tellg();
                break;
        }
    } catch (...) {
        return ERROR_UNKNOWN;
    }
    return status;
}
NAME_SPACE_END()