//
// Created by 常笑男 on 2023/12/28.
//

#ifndef MAIN_IMAGECONST_H
#define MAIN_IMAGECONST_H

#include "Util.h"

NAME_SPACE_START(myUtil)

#ifdef _IMAGE_PNG_
static uint32_t PNG_FLAG = 0x474E5089;
#endif

#ifdef _IMAGE_JPEG_
static uint8_t YQualityTable[64] = {
    16, 11, 10, 16, 24,  40,  51,  61,
    12, 12, 14, 19, 26,  58,  60,  55,
    14, 13, 16, 24, 40,  57,  69,  56,
    14, 17, 22, 29, 51,  87,  80,  62,
    18, 22, 37, 56, 68,  109, 103, 77,
    24, 35, 55, 64, 81,  104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99};

static uint8_t CQualityTable[64]={
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

static const uint8_t bits_dc_luminance[16] = { /* 0-base */
    0, 1, 5, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0
};
static const uint8_t val_dc_luminance[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

static const uint8_t bits_dc_chrominance[16] = { /* 0-base */
    0, 3, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 0, 0
};
static const uint8_t val_dc_chrominance[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

static const uint8_t bits_ac_luminance[16] = { /* 0-base */
    0, 2, 1, 3, 3, 2, 4, 3,
    5, 5, 4, 4, 0, 0, 1, 0x7d
};
static const uint8_t val_ac_luminance[] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa
};

static const uint8_t bits_ac_chrominance[16] = { /* 0-base */
    0, 2, 1, 2, 4, 4, 3, 4,
    7, 5, 4, 4, 0, 1, 2, 0x77
};
static const uint8_t val_ac_chrominance[] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa
};

static const int Zig[64]={
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

static const uint8_t APP[18]={
    0xff,0xe0,0x00,0x10,0x4a,
    0x46,0x49,0x46,0x00,0x01,
    0x01,0x00,0x00,0x01,0x00,
    0x01,0x00,0x00
};

// 段类型
enum JPEGPType {
    SOF0 = 0xC0, // 帧开始
    SOF1 = 0xC1, // 帧开始
    SOF2 = 0xC2, // 帧开始
    DHT = 0xC4,  // 哈夫曼表
    SOI = 0xD8,  // 文件头
    EOI = 0xD9,  // 文件尾
    SOS = 0xDA,  // 扫描行开始
    DQT = 0xDB,  // 定义量化表
    DRI = 0xDD,  // 定义重新开始间隔
    APP0 = 0xE0, // 定义交换格式和图像识别信息
    APP1 = 0xE1, // 定义交换格式和图像识别信息
    APP2 = 0xE2, // 定义交换格式和图像识别信息
    COM = 0xFE,  // 注释
    FLAG = 0xFF  // 开始
};
#endif

enum Operator{
    Sobel		=0,
    Roberts		=1,
    Prewitt		=2,
    Laplacian	=3,
    Canny		=4
};

#ifdef _IMAGE_BMP_
/* Bitmap Header, 54 Bytes  */
static unsigned char BmpHeader[54] =
    {
        0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x60, 0xCD, 0x04, 0x00, 0x23, 0x2E, 0x00, 0x00, 0x23, 0x2E, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
#endif

static double Prewitt1[3][3]={{-1,-1,-1},{0,0,0},{1,1,1}};
static double Prewitt2[3][3]={{-1,0,1},{-1,0,1},{-1,0,1}};

static double Roberts1[3][3]={{1,0},{0,-1}};
static double Roberts2[3][3]={{0,1},{-1,0}};

static double Sobel1[3][3]={{-1,-2,1},{0,0,0},{1,2,1}};
static double Sobel2[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};

NAME_SPACE_END()
#endif // MAIN_IMAGECONST_H