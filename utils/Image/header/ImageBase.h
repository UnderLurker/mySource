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

    virtual bool read(const char* filePath) = 0;
    virtual bool write(const char* filePath) = 0;

    [[nodiscard]] virtual int32_t getWidth() const = 0;
    [[nodiscard]] virtual int32_t getHeight() const = 0;
    [[nodiscard]] virtual vector<RGB**> getRGB() const = 0; // 获取MCU形式的RGB
    virtual Matrix<RGB> getRGBMatrix() = 0; // 获取通用的RGB数据
};

NAME_SPACE_END()

#endif // MAIN_IMAGEBASE_H
