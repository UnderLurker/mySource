//
// Created by Administrator on 2024/3/1.
//

#ifndef MAIN_TEXTURE_H
#define MAIN_TEXTURE_H

#include <glad/glad.h>

#include "Util.h"

NAME_SPACE_START(myUtil)

enum TextureType {
    TEXTURE_1D = 0x0DE0,
    TEXTURE_2D = 0x0DE1
};

class Texture {
public:
    Texture()
        : Texture(myUtil::TEXTURE_2D) {}
    explicit Texture(TextureType type);
    virtual ~Texture() { glDeleteTextures(1, &_textureId); }

    void use() const { glBindTexture(_type, _textureId); }
    void setParam(uint32_t name, int32_t value) const {
        glTexParameteri(_type, name, value);
    }
    template<typename... Args>
    void setParam(uint32_t name, int32_t value, Args... args) const {
        setParam(name, value);
        setParam(args...);
    }
    void setImage2D(uint32_t width, uint32_t height, unsigned char* data) const {
        if (_type != TEXTURE_2D) return;
        glTexImage2D(_type, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    }
    void drawTexture(uint32_t texture) const;

public:
    uint32_t _textureId {0};
    TextureType _type {TEXTURE_2D};
    bool _status {false};

private:
};

NAME_SPACE_END()

#endif // MAIN_TEXTURE_H
