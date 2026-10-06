//
// Created by Administrator on 2024/3/1.
//
#include "texture.h"

NAME_SPACE_START(myUtil)

Texture::Texture(myUtil::TextureType type)
    : _type(type) {
    glGenTextures(1, &_textureId);
}

void Texture::drawTexture(uint32_t texture) const {
    glActiveTexture(texture);
    glBindTexture(_type, _textureId);
}
NAME_SPACE_END()
