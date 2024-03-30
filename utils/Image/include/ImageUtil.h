//
// Created by 常笑男 on 2023/12/28.
//

#ifndef MAIN_IMAGEUTIL_H
#define MAIN_IMAGEUTIL_H

#include "Util.h"
#include "Matrix.h"
#define _USE_MATH_DEFINES
#include <cmath>

NAME_SPACE_START(myUtil)

//阈值
#define THRESHOLD 128
#define GAUSSIAN
#define GAUSSIAN_RADIUS 1
#define GAUSSIAN_TEMPLATE_RADIUS 8

enum ImageStatus {
    SUCCESS,            // 成功
    ERROR_UNKNOWN,      // 未知错误
    ERROR_NULLPTR,      // 空指针错误
    ERROR_LIMIT,        // 数值范围错误
    ERROR_FILE_FORMAT,  // 文件格式错误
    ERROR_FILE_OPERATOR,// 文件操作错误
    ERROR_WRITE,        // 文件写入失败
    ERROR_FILE_ENCODE,  // 文件编码错误
    ERROR_FILE_DECODE,  // 文件解码错误
    ERROR_STRUCT_NOT_DEFINE, // 结构体未定义
    ERROR_ENUM,         // 错误枚举
    ERROR_BMP_HEAD,     // BMP头部失败
    ERROR_BMP_PALETTE,  // BMP调色板失败
    ERROR_JPEG_SOI,
    ERROR_JPEG_APP,
    ERROR_JPEG_SOF,
    ERROR_JPEG_DRI,
    ERROR_JPEG_SOS,
    ERROR_JPEG_EOI,
    ERROR_UNCOMPRESS,
    ERROR_METHOD_UNDEFINE,
    ERROR_UNDO_FILTER,
};

#undef RGB
struct RGB {
    uint8_t red { 0 };
    uint8_t green { 0 };
    uint8_t blue { 0 };
    uint8_t alpha { 0 };
    explicit RGB() = default;
    RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0) : red(r), green(g), blue(b), alpha(a) {}
};

static const RGB RGB_BLACK = RGB(0, 0, 0);
static const RGB RGB_WHITE = RGB(255, 255, 255);
static const RGB RGB_RED = RGB(255, 0, 0);
static const RGB RGB_GREEN = RGB(0, 255, 0);
static const RGB RGB_BLUE = RGB(0, 0, 255);

// 放大图像的矩阵，比例大于1且为整数
template<typename T>
Matrix<T> AmplifyMatrix(T** rgb, int width, int height, int scale = 1)
{
    int resWidth = width * scale, resHeight = height * scale;
    Matrix<T> res(resHeight, resWidth);
    for (int i = 0; i < resHeight; i++) {
        for (int j = 0; j < resWidth; j++) {
            res.setValue(i, j, rgb[i / scale][j / scale]);
        }
    }
    return res;
}
template<typename T>
Matrix<RGB> AmplifyMatrix(Matrix<T>& rgb, int scale = 1)
{
    int resWidth = rgb.col * scale, resHeight = rgb.row * scale;
    Matrix<T> res(resHeight, resWidth);
    for (int i = 0; i < resHeight; i++) {
        for (int j = 0; j < resWidth; j++) {
            res.setValue(i, j, rgb.getValue(i / scale, j / scale));
        }
    }
    return res;
}

NAME_SPACE_END()

#endif // MAIN_IMAGEUTIL_H
