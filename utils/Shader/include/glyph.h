//
// Created by 常笑男 on 2025/3/16.
//

#ifndef _GLYPH_H_
#define _GLYPH_H_

#include "color.h"
#include "include/logger_wrapper.h"
#include "texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#define DEFAULT_FONT_SIZE 24
#define DEFAULT_FONT_FAMILY "consola"

namespace myUtil {

template<typename T>
struct Offset {
    T x;
    T y;
};

template<typename T>
struct Size {
    T width;
    T height;
};

using OffsetF = Offset<float>;
using SizeF   = Size<float>;

struct GlyphConfiguration {
    OffsetF position {};
    float scale {};
    Color color;
};

struct Character {
    std::shared_ptr<myUtil::Texture> texture; // 字形纹理的ID
    SizeF size;                               // 字形大小
    OffsetF bearing;                          // 从基准线到字形左部/顶部的偏移值
    uint32_t advance;                         // 原点距下一个字形原点的距离
};

class FontManager {
public:
    using CharCode = unsigned long;
    ~FontManager() {
        if (FT_Done_FreeType(_libFreeType)) {
            LOGE("CharacterManager release libraryFreeType failed.");
        }
    }

    static FontManager* GetInstance();
    static std::vector<CharCode> UTF16ToCodePoints(const std::u16string& utf16_str);
    Character Get(CharCode code);
    void Insert(const std::pair<CharCode, Character>& value);
    void SetFontFamilies(const std::vector<std::string>& families) { _fontFamilies = families; }
    const std::vector<std::string>& GetFontFamilies() const { return _fontFamilies; }
    void AddFontFamily(const std::string& str) { _fontFamilies.push_back(str); }
    void SetCurDir(const std::string dir) { _curDir = dir; }

protected:
    bool LoadChar(CharCode code, FT_Face* face);
    bool AddCharCode(CharCode code);
    bool InitFontLibrary() {
        if (FT_Init_FreeType(&_libFreeType)) {
            LOGE("CharacterManager: Could not init FreeType Library");
            return false;
        }
        return true;
    }

private:
    std::map<CharCode, myUtil::Character> _characters;
    std::vector<std::string> _fontFamilies {DEFAULT_FONT_FAMILY};
    std::string _curDir;
    FT_Library _libFreeType;
    static thread_local std::unique_ptr<FontManager> _instance;
};
} // namespace myUtil

#endif // _GLYPH_H_
