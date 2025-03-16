//
// Created by 常笑男 on 2025/3/16.
//

#include "glyph.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace myUtil {
thread_local std::unique_ptr<CharacterManager> CharacterManager::_instance = nullptr;
CharacterManager* CharacterManager::GetInstance() {
    if (_instance) return _instance.get();
    _instance = std::make_unique<CharacterManager>();
    return _instance.get();
}

void CharacterManager::Insert(const std::pair<GLchar, Character>& value) {
    _characters.insert(value);
}

Character& CharacterManager::Get(GLchar index) {
    auto iter = _characters.find(index);
    if (iter != _characters.end()) return iter->second;
    return _characters[index];
}

void CharacterManager::Init(const std::string& fontName) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        LOGE("ERROR::FREETYPE: Could not init FreeType Library");
    std::string fontFile = "fonts/" + fontName + ".ttf";
    FT_Face face;
    if (FT_New_Face(ft, fontFile.c_str(), 0, &face))
        LOGE("ERROR::FREETYPE: Failed to load font");
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 禁用字节对齐限制
    for (GLubyte c = 0; c < 128; c++) {
        // 加载字符的字形
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            LOGE("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        auto texture = std::make_shared<myUtil::Texture>(myUtil::TextureType::TEXTURE_2D);
        texture->use();
        texture->setImage2D(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
        texture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE,
                          GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE,
                          GL_TEXTURE_MIN_FILTER, GL_LINEAR,
                          GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 储存字符供之后使用
        myUtil::Character character = {
            texture,
            myUtil::SizeF {static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows)},
            myUtil::OffsetF {static_cast<float>(face->glyph->bitmap_left),  static_cast<float>(face->glyph->bitmap_top) },
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Insert(std::pair<GLchar, myUtil::Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
}
}
