//
// Created by 常笑男 on 2023/12/28.
//

#ifndef MAIN_IMAGEBASE_H
#define MAIN_IMAGEBASE_H

#include "Util.h"
#include "ImageUtil.h"
#include <vector>

using namespace std;

NAME_SPACE_START(myUtil)

class Image {
public:
    explicit Image() = default;
    virtual ~Image() = default;

    virtual ImageStatus read(const char* filePath) = 0;
    virtual ImageStatus write(const char* filePath) = 0;

    [[nodiscard]] virtual int32_t getWidth() const = 0;
    [[nodiscard]] virtual int32_t getHeight() const = 0;
    virtual void setWidth(int32_t width) = 0;
    virtual void setHeight(int32_t height) = 0;
    [[nodiscard]] virtual Matrix<RGB> getRGBMatrix() const = 0; // 获取通用的RGB数据
    virtual void setRGBMatrix(const Matrix<RGB>&) = 0; // 设置通用的RGB数据
};

NAME_SPACE_END()

#endif // MAIN_IMAGEBASE_H