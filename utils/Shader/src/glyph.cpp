//
// Created by 常笑男 on 2025/3/16.
//

#include "glyph.h"

#include "include/logger_wrapper.h"

namespace myUtil {
thread_local std::unique_ptr<FontManager> FontManager::_instance = nullptr;
FontManager* FontManager::GetInstance() {
    if (_instance) return _instance.get();
    auto tmp = std::make_unique<FontManager>();
    if (tmp->InitFontLibrary()) {
        _instance = std::move(tmp);
        return _instance.get();
    }
    return nullptr;
}
std::vector<FontManager::CharCode> FontManager::UTF16ToCodePoints(const std::u16string& utf16_str) {
    std::vector<FontManager::CharCode> codepoints;
    for (size_t i = 0; i < utf16_str.size(); ) {
        FontManager::CharCode codepoint;
        char16_t c1 = utf16_str[i];
        
        if ((c1 & 0xFC00) == 0xD800) {  // 高代理对（0xD800-0xDBFF）
            if (i + 1 >= utf16_str.size()) break;  // 无效代理对
            char16_t c2 = utf16_str[i + 1];
            if ((c2 & 0xFC00) == 0xDC00) {  // 低代理对（0xDC00-0xDFFF）
                codepoint = 0x10000 + ((c1 - 0xD800) << 10) + (c2 - 0xDC00);
                i += 2;
            } else {
                i++;  // 无效代理对，跳过
            }
        } else {
            codepoint = c1;
            i++;
        }
        codepoints.push_back(codepoint);
    }
    return codepoints;
}

void FontManager::Insert(const std::pair<CharCode, Character>& value) {
    _characters.insert(value);
}

Character FontManager::Get(CharCode code) {
    auto iter = _characters.find(code);
    if (iter != _characters.end()) return iter->second;
    return AddCharCode(code) ? _characters[code] : Character();
}

bool FontManager::LoadChar(CharCode code, FT_Face* face) {
    for (auto fontFamily : _fontFamilies) {
        std::string fontFile = _curDir + "/fonts/" + fontFamily + ".ttf";
        if (FT_New_Face(_libFreeType, fontFile.c_str(), 0, face)) {
            FT_Done_Face(*face);
            continue;
        }
        FT_Set_Pixel_Sizes(*face, 0, DEFAULT_FONT_SIZE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 禁用字节对齐限制
        if (FT_Load_Char(*face, code, FT_LOAD_RENDER)) {
            FT_Done_Face(*face);
            continue;
        }
        return true;
    }
    return false;
}

bool FontManager::AddCharCode(CharCode code) {
    FT_Face face;
    if (!LoadChar(code, &face)) {
        LOGE("CharacterManager: Failed to load Glyph");
        return false;
    }
    auto texture = std::make_shared<myUtil::Texture>(myUtil::TextureType::TEXTURE_2D);
    texture->use();
    texture->setImage2D(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
    texture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    myUtil::Character character = {
        texture,
        myUtil::SizeF {static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows)},
        myUtil::OffsetF {static_cast<float>(face->glyph->bitmap_left),  static_cast<float>(face->glyph->bitmap_top) },
        static_cast<GLuint>(face->glyph->advance.x)
    };
    _characters.insert({code, std::move(character)});
    FT_Done_Face(face);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    return true;
}
} // namespace myUtil
