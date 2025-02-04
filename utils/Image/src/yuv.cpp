//
// Created by 常笑男 on 2024/12/15.
//

#include "yuv.h"

namespace myUtil {
namespace {
constexpr uint32_t YUV_SIZE = 3;

double** CreateArray(uint32_t width, uint32_t height) {
    auto array = new double*[height];
    for (uint32_t i = 0; i < height; i++) {
        array[i] = new double[width];
    }
    return array;
};
double*** CreateYUVByType(uint32_t width, uint32_t height, SampleType type) {
    auto yuv     = new double**[YUV_SIZE];
    yuv[Y_INDEX] = CreateArray(width, height);
    switch (type) {
        case YUV411: {
            yuv[U_INDEX] = CreateArray(width / 4, height);
            yuv[V_INDEX] = CreateArray(width / 4, height);
            break;
        }
        case YUV420: {
            yuv[U_INDEX] = CreateArray(width / 2, height / 2);
            yuv[V_INDEX] = CreateArray(width / 2, height / 2);
            break;
        }
        case YUV422: {
            yuv[U_INDEX] = CreateArray(width / 2, height);
            yuv[V_INDEX] = CreateArray(width / 2, height);
            break;
        }
        case YUV444: {
            yuv[U_INDEX] = CreateArray(width, height);
            yuv[V_INDEX] = CreateArray(width, height);
            break;
        }
    }
    return yuv;
}
} // namespace
double*** RGB2YUV(const RGB** rgb, uint32_t width, uint32_t height, SampleType type) {
    auto yuv = CreateYUVByType(width, height, type);

    return yuv;
}

RGB** YUV2RGB(double*** yuv, uint32_t width[], uint32_t height[], SampleType type) {}
} // namespace myUtil
