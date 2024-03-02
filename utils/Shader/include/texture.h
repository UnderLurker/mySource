//
// Created by Administrator on 2024/3/1.
//

#ifndef MAIN_TEXTURE_H
#define MAIN_TEXTURE_H

#include "Util.h"

NAME_SPACE_START(myUtil)

enum TextureType {
    TEXTURE_1D = 0x0DE0,
    TEXTURE_2D = 0x0DE1
};

class Texture {
public:
    Texture() = delete;
    explicit Texture(TextureType type);
    virtual ~Texture() = default;

    void setParam(uint32_t name, int32_t value) const;
    void bindTexture(uint32_t texture) const;

public:
    uint32_t _textureId {0};
    TextureType _type {TEXTURE_2D};
    bool _status {false};

private:
};

NAME_SPACE_END()

#endif // MAIN_TEXTURE_H
