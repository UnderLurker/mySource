#ifndef _IMAGE_PNG_
#define _IMAGE_PNG_

#include <cstdint>
#include <fstream>
#include <list>
#include <vector>

#include "ImageBase.h"
#include "ImageConst.h"
#include "ImageUtil.h"
#include "Matrix.h"
#include "Util.h"

// https://www.w3.org/TR/png-3

NAME_SPACE_START(myUtil)
#define _DEBUG_
// #define _IDAT_DEBUG_
#define TEMP_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__);
namespace CHUNK {
enum ChunkType {
    IDAT = 0x49444154,
    IEND = 0x49454E44,
    IHDR = 0x49484452,
    PLTE = 0x504C5445,
    acTL = 0x6163544C,
    bKGD = 0x624B4744,
    cHRM = 0x6348524D,
    cICP = 0x63494350,
    cLLi = 0x634C4C69,
    eXIf = 0x65584966,
    fcTL = 0x6663544C,
    fdAT = 0x66644154,
    gAMA = 0x67414D41,
    hIST = 0x68495354,
    iCCP = 0x69434350,
    iTXt = 0x69545874,
    mDCv = 0x6D444376,
    pHYs = 0x70485973,
    sBIT = 0x73424954,
    sPLT = 0x73504C54,
    sRGB = 0x73524742,
    tEXt = 0x74455874,
    tIME = 0x74494D45,
    tRNS = 0x74524E53,
    zTXt = 0x7A545874,
};

static map<uint32_t, string> typeMap {
    {IDAT, "IDAT"},
    {IEND, "IEND"},
    {IHDR, "IHDR"},
    {PLTE, "PLTE"},
    {acTL, "acTL"},
    {bKGD, "bKGD"},
    {cHRM, "cHRM"},
    {cICP, "cICP"},
    {cLLi, "cLLi"},
    {eXIf, "eXIf"},
    {fcTL, "fcTL"},
    {fdAT, "fdAT"},
    {gAMA, "gAMA"},
    {hIST, "hIST"},
    {iCCP, "iCCP"},
    {iTXt, "iTXt"},
    {mDCv, "mDCv"},
    {pHYs, "pHYs"},
    {sBIT, "sBIT"},
    {sPLT, "sPLT"},
    {sRGB, "sRGB"},
    {tEXt, "tEXt"},
    {tIME, "tIME"},
    {tRNS, "tRNS"},
    {zTXt, "zTXt"},
};

enum FilterType {
    NONE = 0,
    SUB,
    UP,
    AVERAGE,
    PAETH
};

enum ColorType {
    GREY_SCALE     = 0,
    RGB            = 2,
    INDEXED_COLOUR = 3,
    GREY_ALPHA     = 4,
    RGBA           = 6
};

static map<ColorType, uint32_t> ColorTypeBitMap {
    {GREY_SCALE,     1},
    {RGB,            3},
    {INDEXED_COLOUR, 1},
    {GREY_ALPHA,     2},
    {RGBA,           4}
};
}

void convertSmall16(uint16_t& value);
void convertSmall32(uint32_t& value);

struct ChunkHead {
    uint32_t length;
    uint32_t chunkType;
    void reset() {
        length    = 0;
        chunkType = 0;
    }
};

// 先由PNGData解析type，然后通过多态解析
class Chunk {
    struct CRC {
        uint32_t crc;
    };

public:
    CRC crc {};
    virtual ImageStatus decode(fstream& file, uint32_t length) { return SUCCESS; }
    [[nodiscard]] uint32_t CRC() const { return crc.crc; }
    template<typename T>
    void readQueue(fstream& file, uint32_t length, uint32_t& curPos, queue<T>& q) {
        for (; curPos < length; curPos++) {
            T ch;
            file.read((char*)&ch, sizeof(ch));
            if (ch == 0) break;
            else q.push(ch);
        }
    }
};

struct ChunkColour {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
};

class IHDRChunk : public Chunk {
    struct IHDRData {
        uint32_t width;
        uint32_t height;
        uint8_t bitDepth;
        uint8_t colorType;
        uint8_t compressionMethod;
        uint8_t filterMethod;
        uint8_t interlaceMethod;
    };

public:
    IHDRData _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
    [[nodiscard]] uint32_t width() const { return _data.width; }
    [[nodiscard]] uint32_t height() const { return _data.height; }
    [[nodiscard]] uint8_t bitDepth() const { return _data.bitDepth; }
    [[nodiscard]] uint8_t colorType() const { return _data.colorType; }
    [[nodiscard]] uint8_t compressionMethod() const { return _data.compressionMethod; }
    [[nodiscard]] uint8_t filterMethod() const { return _data.filterMethod; }
    [[nodiscard]] uint8_t interlaceMethod() const { return _data.interlaceMethod; }
    [[nodiscard]] uint32_t minCompressDataLength() const;
};

class PLTEChunk : public Chunk {
    struct PLTEData {
        uint8_t rgbRed;
        uint8_t rgbGreen;
        uint8_t rgbBlue;
    };

public:
    vector<PLTEData> _data;
    ImageStatus decode(fstream& file, uint32_t length) override;
    vector<PLTEData>& palette() { return _data; }
};

class tRNSChunk : public Chunk {
    struct tRNSData {
        uint16_t colourType0;
        ChunkColour colourType2;
        vector<uint8_t> colourType3;
    };
    tRNSData _data;

public:
    // Greyscale	0
    // Truecolour	2
    // Indexed-colour	3
    // Greyscale with alpha	4
    // Truecolour with alpha	6
    uint32_t colourType {0};
    ImageStatus decode(fstream& file, uint32_t length) override;
    tRNSData& tRNS() { return _data; }
};

class cHRMChunk : public Chunk {
    struct cHRMData {
        uint32_t whitePointX;
        uint32_t whitePointY;
        uint32_t redX;
        uint32_t redY;
        uint32_t greenX;
        uint32_t greenY;
        uint32_t blueX;
        uint32_t blueY;
    };

public:
    cHRMData _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class gAMAChunk : public Chunk {
public:
    uint32_t _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class iCCPChunk : public Chunk {
    struct iCCPData {
        char profileName[80];
        uint8_t compressMethod;
        vector<uint8_t> compressProfile;
    };

public:
    iCCPData _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class sBITChunk : public Chunk {
    struct SignificantColour {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };
    struct SignificantGrey {
        uint8_t greyScale;
        uint8_t alpha;
    };
    struct SignificantColourAll {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    };
    union sBITData {
        uint8_t colourType0;
        SignificantColour colourType23;
        SignificantGrey colourType4;
        SignificantColourAll colourType6;
    };

public:
    uint32_t colourType {0};
    sBITData _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class sRGBChunk : public Chunk {
public:
    uint8_t _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class cICPChunk : public Chunk {
    struct cICPData {
        uint8_t colourPrimaries;
        uint8_t transferFunction;
        uint8_t matrixCoefficients;
        uint8_t videoFullRangeFlag;
    };

public:
    cICPData _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class mDCvChunk : public Chunk {
    struct mDCvData {
        uint16_t chromaticityX1;
        uint16_t chromaticityY1;
        uint16_t chromaticityX2;
        uint16_t chromaticityY2;
        uint16_t chromaticityX3;
        uint16_t chromaticityY3;
        uint16_t whitePointX;
        uint16_t whitePointY;
        uint32_t maxLum;
        uint32_t minLum;
    };

public:
    struct Chromaticity {
        float x;
        float y;
    };
    mDCvData _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
    [[nodiscard]] vector<Chromaticity> chromaticity() const;
    Chromaticity whitePoint();
    float maxLum();
    float minLum();
};

class cLLiChunk : public Chunk {
    uint32_t MaxCLL {};
    uint32_t MaxFALL {};

public:
    ImageStatus decode(fstream& file, uint32_t length) override;
    [[nodiscard]] uint32_t maxCLL() const { return MaxCLL * 0.0001; }
    [[nodiscard]] uint32_t maxFALL() const { return MaxFALL * 0.0001; }
};

class tEXtChunk : public Chunk {
public:
    char keyWord[80];
    string textString;
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class zTXtChunk : public Chunk {
public:
    char keyWord[80];
    uint8_t compressMethod;
    vector<uint8_t> compressDataStream;
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class iTXtChunk : public Chunk {
public:
    char keyWord[80];
    uint8_t compressFlag;
    uint8_t compressMethod;
    string languageTag;
    unique_ptr<uint8_t[]> translatedKeyWord;
    unique_ptr<uint8_t[]> text;
    ImageStatus decode(fstream& file, uint32_t length) override;

private:
    void tagProcess(fstream& file, uint32_t length, uint32_t& curPos);
    void tKeyWordProcess(fstream& file, uint32_t length, uint32_t& curPos);
    void textProcess(fstream& file, uint32_t length, uint32_t& curPos);
};

class bKGDChunk : public Chunk {
    union bKGDData {
        uint16_t greyScale;
        ChunkColour colour;
        uint8_t paletteIndex;
    };

public:
    uint32_t colourType {0};
    bKGDData _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class hISTChunk : public Chunk {
public:
    unique_ptr<uint16_t[]> _data;
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class pHYsChunk : public Chunk {
public:
    uint32_t pixelsPerUnitX {};
    uint32_t pixelsPerUnitY {};
    // 0:unit is unknown  1:unit is the metre
    uint8_t unit {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class sPLTChunk : public Chunk {
    struct PaletteEntry {
        uint16_t red;
        uint16_t green;
        uint16_t blue;
        uint16_t alpha;
    };

public:
    string paletteName;
    // 8 or 16
    uint8_t sampleDepth;
    unique_ptr<PaletteEntry[]> _entry;
    ImageStatus decode(fstream& file, uint32_t length) override;

private:
    void nameProcess(fstream& file, uint32_t length, uint32_t& curPos);
    static void paletteEntryProcess(fstream& file, PaletteEntry& entry);
};

class eXIfChunk : public Chunk {
public:
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class tIMEChunk : public Chunk {
public:
    uint16_t year {};
    uint8_t month {};
    uint8_t day {};
    uint8_t hour {};
    uint8_t minute {};
    uint8_t second {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class acTLChunk : public Chunk {
    struct acTLData {
        uint32_t numFrames;
        uint32_t numPlays;
    };

public:
    acTLData _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class fcTLChunk : public Chunk {
    struct fcTLData {
        uint32_t sequenceNumber;
        uint32_t width;
        uint32_t height;
        uint32_t offsetX;
        uint32_t offsetY;
        uint16_t delayNum;
        uint16_t delayDen;
    };

public:
    uint8_t disposeOp {};
    uint8_t blendOp {};
    fcTLData _data {};
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class fdATChunk : public Chunk {
public:
    uint32_t sequenceNumber;
    unique_ptr<uint8_t[]> frameData;
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class IDATChunk : public Chunk {
public:
    ImageStatus decode(fstream& file, uint32_t length, std::vector<std::pair<long, long>>& IDATMap);
};

class IENDChunk : public Chunk {
public:
    ImageStatus decode(fstream& file, uint32_t length) override;
};

class PNGData : public Image {
    string _filePath;
    uint32_t _formatFlag {};
    int32_t _width {0};
    int32_t _height {0};
    uint32_t _minCompressDataLength {1};
    long _iDATLen {0}; // idat compressed data length (total)
    Matrix<RGB>* _rgb {nullptr};
    IHDRChunk _IHDR;
    IENDChunk _IEND;
    unordered_map<uint32_t, Chunk*> _ancillaryChunk;
    unique_ptr<uint8_t[]> _data {nullptr};
    std::vector<std::pair<long, long>> _IDATMap; // pair<pos, length>

public:
    explicit PNGData() = default;
    ~PNGData() override {
        delete _rgb;
        _rgb = nullptr;
        for (auto& item : _ancillaryChunk) {
            delete item.second;
            item.second = nullptr;
        }
    }

    ImageStatus read(const char* filePath) override;
    ImageStatus write(const char* filePath) override { return SUCCESS; }

    [[nodiscard]] int32_t getWidth() const override { return _width; }
    [[nodiscard]] int32_t getHeight() const override { return _height; }
    void setWidth(int32_t width) override { _width = width; }
    void setHeight(int32_t height) override { _height = height; }
    [[nodiscard]] Matrix<RGB> getRGBMatrix() const override { return *_rgb; } // 获取通用的RGB数据
    void setRGBMatrix(const Matrix<RGB>& rgb) override { *_rgb = rgb; }       // 设置通用的RGB数据
private:
    [[maybe_unused]] bool checkFormat() override { return _formatFlag == PNG_FLAG; }
    ImageStatus decodeProcess(fstream& file, const ChunkHead& head);
    ImageStatus ancillaryChunkFactory(fstream& file, const ChunkHead& head);
    ImageStatus decodeIDATDataProcess(fstream& file);
    ImageStatus getIDATData(fstream& file);
    ImageStatus uncompress_UndoFilter();
    bool undoFilter0(unique_ptr<uint8_t[]>& uncompressData, unsigned long length);
    void png_undo_filter_sub(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) const;
    void png_undo_filter_up(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length);
    void png_undo_filter_average(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length) const;
    void png_undo_filter_paeth(unique_ptr<uint8_t[]>& uncompressData, uint32_t curPos, uint32_t length);
    void data2Matrix(unique_ptr<uint8_t[]>& uncompressData);
};

NAME_SPACE_END()

#endif // !_IMAGE_PNG_
