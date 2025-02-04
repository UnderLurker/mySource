//
// Created by 常笑男 on 2024/12/15.
//

#ifndef _YUV_H
#define _YUV_H

#include "ImageUtil.h"
namespace myUtil {
#define Y_INDEX 0
#define U_INDEX 1
#define V_INDEX 2

#define Y4 (3 << 4)
#define U1 (1 << 2)
#define U2 (2 << 2)
#define U4 (3 << 2)
#define V0 (0)
#define V1 (1)
#define V2 (2)
#define V4 (3)

enum SampleType {
    YUV411 = Y4 | U1 | V1,
    YUV420 = Y4 | U2 | V0,
    YUV422 = Y4 | U2 | V2,
    YUV444 = Y4 | U4 | V4,
};
/**
 * rgb to yuv
 * @param rgb input rgb
 * @param width rgb width
 * @param height rgb height
 * @param type sampleType
 * @return a array [Y, U, V]
 */
double*** RGB2YUV(const RGB** rgb, uint32_t width, uint32_t height, SampleType type);
/**
 * yuv to rgb
 * @param yuv
 * @param width width array of YUV
 * @param height height array of YUV
 * @param type sampleType
 * @return result rgb
 */
RGB** YUV2RGB(double*** yuv, uint32_t width[], uint32_t height[], SampleType type);

} // namespace myUtil

#endif // _YUV_H
