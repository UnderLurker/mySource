//
// Created by 常笑男 on 2024/6/20.
//

#ifndef _ID3V1_H
#define _ID3V1_H

#include <cassert>

#include "music_const.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

#define ID3V1_LEN 128

class ID3V1 {
public:
    void setValue(const uint8_t* data, size_t length) {
        MYASSERT(length == ID3V1_LEN);
        memcpy(tagFlag, data, 3);
        memcpy(title, data + 3, 30);
        memcpy(artist, data + 33, 30);
        memcpy(album, data + 63, 30);
        memcpy(years, data + 93, 4);
        memcpy(comment, data + 97, 28);
        reserve = *(data + 125);
        track   = *(data + 126);
        genre   = *(data + 127);
    }

    [[nodiscard]] std::string getGenre() const { return GenreMap[genre]; }

public:
    char tagFlag[3];
    char title[30];
    char artist[30];
    char album[30];
    char years[4];
    char comment[28];
    uint8_t reserve;
    uint8_t track;
    uint8_t genre;
};

NAME_SPACE_END()

#endif // _ID3V1_H
