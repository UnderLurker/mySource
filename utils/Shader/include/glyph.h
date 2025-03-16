//
// Created by 常笑男 on 2025/3/16.
//

#ifndef _GLYPH_H_
#define _GLYPH_H_

#include "color.h"
#include "texture.h"

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
    SizeF size;                              // 字形大小
    OffsetF bearing;                          // 从基准线到字形左部/顶部的偏移值
    uint32_t advance;                           // 原点距下一个字形原点的距离
};

class CharacterManager {
public:
    static CharacterManager* GetInstance();
    void Insert(const std::pair<GLchar, Character>& value);
    Character& Get(GLchar index);
    void Init(const std::string& fontName);
    void Add(wchar_t index);

private:
    std::map<GLchar, myUtil::Character> _characters;
    std::map<wchar_t, myUtil::Character> _wCharacters;
    static thread_local std::unique_ptr<CharacterManager> _instance;
};
} // namespace myUtil

#endif // _GLYPH_H_
