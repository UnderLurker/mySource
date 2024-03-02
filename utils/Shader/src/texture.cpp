//
// Created by Administrator on 2024/3/1.
//
#include "texture.h"

#include <glad/glad.h>
NAME_SPACE_START(myUtil)

Texture::Texture(myUtil::TextureType type) {
    glGenTextures(1, &_textureId);
    _type = type;
}

void Texture::setParam(uint32_t name, int32_t value) const {
    glBindTexture(_type, _textureId);
    glTexParameteri(_type, name, value);
}

void Texture::bindTexture(uint32_t texture) const {
    glActiveTexture(texture);
    glBindTexture(_type, _textureId);
}
NAME_SPACE_END()
