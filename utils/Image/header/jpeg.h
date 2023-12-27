//
// Created by 常笑男 on 2023/12/28.
//

#ifndef MAIN_JPEG_H
#define MAIN_JPEG_H

#include "Util.h"
#include "ImageUtil.h"
#include "ImageBase.h"
#include <cstdint>
#include <fstream>
#include <cstdint>
#include <utility>
#include <string>
#include <vector>
#include <iostream>
using namespace std;
NAME_SPACE_START(myUtil)

#define ROW 8
#define COL 8
#define HUFFMAN_DECODE_DEQUE_CACHE 64//单位：位
// #define _DEBUG_
// #define _DEBUGOUT_
// #define _WRITE_DEBUG_

// 将一维数组变为二维数组
double** UnZigZag(const int* originArray);

// SOS
class JPEGScan {
public:
    // componentId,<DC,AC>
    map<uint8_t, pair<uint8_t, uint8_t>> componentHuffmanMap;
    bool Init(fstream& file, uint16_t len);
    //    string to_string();
};

// APP
class JPEGInfo {
public:
    uint16_t version;
};
// DHT
class JPEGHuffmanCode {
public:
    using iterator = map<uint16_t, pair<uint8_t, uint8_t>>::iterator;
    //<code,<bit,weight>
    map<uint16_t, pair<uint8_t, uint8_t>> table;
    // init huffman table
    bool Init(fstream& file, uint16_t len);
    // find-true not find-false
    bool findKey(const uint16_t& code, const uint8_t& bit, iterator& it);
    static void write(fstream& file);
};
// 编码Huffman表
struct JPEGHuffmanEncode {
    //<weight,<code,bit>
    map<uint8_t, pair<uint16_t, uint8_t>> table;
};
// DQT
// quality table
class JPEGQuality {
public:
    uint8_t precision;
    uint8_t id;
    vector<uint16_t> table;
    bool Init(fstream& file, uint16_t len);
    static void write(fstream& file);
};

// SOF segment
class JPEGComponent {
public:
    // 1＝Y, 2＝Cb, 3＝Cr, 4＝I, 5＝Q
    uint8_t colorId;
    uint8_t h_samp_factor;
    uint8_t v_samp_factor;
    uint8_t qualityId;
    bool Init(fstream& file, uint16_t len);
};

class JPEGData : public Image {
    int32_t bitCurPos = 0;   // 当前字节在哪个bit位
    int32_t curBitValue = 0; // 当前值是多少
    int32_t max_h_samp_factor; // 行MCU
    int32_t max_v_samp_factor; // 列MCU
    int32_t width;             // 图片宽度
    int32_t height;            // 图片高度
    int32_t mcu_rows;          // 有多少行mcu
    int32_t mcu_cols;          // 有多少列mcu
    int32_t precision;         // 精度
    bool isYUV411 = false;
    bool isYUV422 = false;
    bool isYUV111 = false;
    uint8_t curDRI = 0;                 // 当前重置直流分量标识,这里只取个位方便计算
    uint16_t resetInterval = 0;         // 单位是MCU
    int32_t preDCValue[3] = { 0 };      // 用于直流差分矫正
    vector<JPEGQuality> quality;        // 量化表
    vector<JPEGHuffmanCode> dc_huffman; // 解码huffman码表
    vector<JPEGHuffmanCode> ac_huffman;
    vector<JPEGHuffmanEncode> en_dc_huffman; // 编码Huffman表
    vector<JPEGHuffmanEncode> en_ac_huffman;
    vector<JPEGComponent> component; // component每个颜色分量
    JPEGScan scan;
    vector<double**> ycbcr;
    vector<RGB**> rgb; // 解析出来的rgb信息
    double** DCTAndIDCTArray;
    streampos pos;                           // 文件的总大小
    bool EOI { false };                      // 是否
    int32_t write_samp_factor[3][2] = { 0 }; // 生成图片的采样因子
public:
    explicit JPEGData() : max_h_samp_factor(0), max_v_samp_factor(0), width(0), height(0), precision(0)
    {
        mcu_rows = mcu_cols = 0;
        DCTAndIDCTArray = createDCTAndIDCTArray(ROW);
    }
    ~JPEGData() override
    {
        FREE_LP_2(DCTAndIDCTArray, ROW - 1)
        FREE_VECTOR_LP(rgb, ROW)
    }
    // 解析图片
    bool read(const char* filePath) override;
    // 生成图片,采样因子h v
    bool write(const char* filePath) override { return true; }
    bool writeJPEG(const char* filePath, int32_t samp_factor[3][2], int32_t quality_scale);

    [[nodiscard]] int32_t getWidth() const override { return width; }
    [[nodiscard]] int32_t getHeight() const override { return height; }
    [[nodiscard]] int32_t getMaxHSampFactor() const { return max_h_samp_factor; }
    [[nodiscard]] int32_t getMaxVSampFactor() const { return max_v_samp_factor; }
    [[nodiscard]] vector<RGB**> getRGB() const override { return rgb; } // 获取MCU形式的RGB
    Matrix<RGB> getRGBMatrix() override; // 获取通用的RGB数据

private:
    void writeBitToFile(fstream& file, int32_t len, int32_t realData);
    void writeBit(fstream& file, int32_t len, int32_t realData, pair<uint16_t, uint8_t>& huffmanCode);
    double** createDCTAndIDCTArray(int32_t row);
    void DCT(double** originMatrix);
    void IDCT(double** originMatrix);
    bool encodeProcess(fstream& file, uint16_t& pLen, uint16_t pType);
    bool SOFProcess(fstream& file, uint16_t pLen);
    bool DHTProcess(fstream& file, uint16_t pLen);
    bool SOSProcess(fstream& file, uint16_t pLen);
    bool DQTProcess(fstream& file, uint16_t pLen);
    void DRIProcess(fstream& file, uint16_t pLen);
    void initQualityTable(int32_t quality_scale);
    bool readSOF(fstream& file, uint16_t len);
    bool readData(fstream& file);
    bool huffmanDecode(fstream& file);

    RGB** YCbCrToRGB(const int32_t* YUV);
    void RGBToYCbCr(Matrix<RGB> _rgb, fstream& file);

    // 标记位检查 是否结束,是否重置直流矫正数值，返回要添加的数值
    string FlagCheck(fstream& file, int32_t byteInfo);
    uint16_t ReadByte(fstream& file, int32_t len);
    uint16_t findHuffmanCodeByBit(
        fstream& file, int32_t& length, int32_t& pos, string& deque, int32_t curValue, int32_t& curValLen);
    void createDCEnHuffman();
    void createACEnHuffman();
};

NAME_SPACE_END()

#endif // MAIN_JPEG_H
