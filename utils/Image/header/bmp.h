#pragma once
#include <cstdint>
#include <fstream>
#include <utility>
#ifndef _IMAGE_
#define _IMAGE_

#include <iostream>
#include <string>
#include <vector>

#include "ImageBase.h"
#include "ImageUtil.h"
#include "Matrix.h"
#include "Util.h"
using namespace std;

NAME_SPACE_START(myUtil)

struct Palette {
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbAlpha;
};

struct BMPHeader {
    uint32_t bfHeadSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
    uint32_t biHeadSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes = 1;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};

// 此类只编码倒立图像，即height>0
class BMPData : public Image {
    int32_t _dataSize { 0 }; // 数据总大小
    int32_t _rowSize { 0 };  // 一行有多少个字节（是4的整数倍）
    int32_t _width { 0 };
    int32_t _height { 0 };
    int32_t _offBits { 54 };               // 文件头到位图数据的偏移量
    int32_t _flag { 0 };                   // 图像使用哪一通道，0三通道，1 blue，2 green，3 red
    Matrix<RGB> _buf;                      // rgb颜色信息
    Matrix<uint8_t>* _grayBuf { nullptr }; // 灰度图信息
    bool _gray { false };                  // false 灰度有调色板 true 彩色没有调色板
    bool _forward { true };                // true 为height>0数据表示从图像左下到右上
    uint8_t* _bitmap { nullptr };          // 最终的数据
    bool _isEncode { false };               // 是否为编码
    vector<Palette> _palettes; // 调色板信息
    BMPHeader _bmpHeader {};

public:
    BMPData() = default;
    // 重新编码 _gray false灰度 true24位彩色这个gray是决定最后生成的图像是否为灰度图像
    BMPData(const Matrix<RGB>& buf, int32_t width, int32_t height, bool gray = false)
        : _buf(buf), _width(width), _height(height), _gray(gray)
    {
        Init();
        _isEncode = true;
        if (_gray)
            _grayBuf = new Matrix<uint8_t>(_height, _width, 0);
    }
    ~BMPData() override
    {
        if (_bitmap)
            delete[] _bitmap;
        if (_grayBuf)
            delete _grayBuf;
    }
    ImageStatus read(const char* filePath) override;
    ImageStatus write(const char* filePath) override;
    Matrix<uint8_t> getGray() { return *_grayBuf; }
    [[nodiscard]] int32_t getWidth() const override { return _width; };
    [[nodiscard]] int32_t getHeight() const override { return _height; };
    void setWidth(int32_t width) override { _width = width; }
    void setHeight(int32_t height) override { _height = height; }
    [[nodiscard]] Matrix<RGB> getRGBMatrix() const override; // 获取通用的RGB数据
    void setRGBMatrix(const Matrix<RGB>& rgb) override { _buf = rgb; }

    // 灰度化,对还没有转成位图数据的
    void GrayEncoder(
        double (*convert)(double) = [](double in) { return in; },
        double (*GrayAlgorithm)(RGB) = [](RGB in) { return (in.blue + in.red + in.green) / 3.0; });
    // 高斯模糊处理，isRGB表示当前是对彩色图像还是对灰度图像进行处理
    void GaussianHandle(
        bool isRGB, double (*convert)(double) = [](double in) { return in; }, int32_t flag = 0);
    // 边缘检测,只对灰度图像
    void EdgeDetect(double matrix1[3][3], double matrix2[3][3], int32_t row, uint8_t (*algorithm)(double, double));

private:
    void Init();
    void SetBitmapInfo(int32_t _dataSize, int32_t _height, int32_t _width);
    bool HeadProcess(fstream& file);
    bool PaletteProcess(fstream& file);
    bool DataProcess(fstream& file);

    void analysisHead(uint8_t* head);
};

NAME_SPACE_END()

#endif //!_IMAGE_
