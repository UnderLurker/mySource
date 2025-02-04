//
// Created by 常笑男 on 2024/8/11.
//

#ifndef _WEBP_H
#define _WEBP_H

#include "ImageBase.h"

namespace myUtil {

struct WebPChunk {
    uint32_t fourcc;
    uint32_t chunkSize;
    const uint8_t* chunkData;
};

class WebPData : public Image {
public:
    bool checkFormat() override;
    int32_t getWidth() const override;
    int32_t getHeight() const override;
    Matrix<RGB> getRGBMatrix() const override;

    void setWidth(int32_t width) override;
    void setHeight(int32_t height) override;
    void setRGBMatrix(const Matrix<myUtil::RGB>&) override;

    ImageStatus read(const char* filePath) override;
    ImageStatus write(const char* filePath) override;

private:
    ImageStatus processData();
    ImageStatus processHeader();

private:
    uint8_t* _cache {nullptr};
    uint32_t _fileSize {0};
    uint32_t _position {0};
    bool _loss {false};
};
} // namespace myUtil

#endif // _WEBP_H
