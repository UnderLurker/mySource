//
// Created by 常笑男 on 2024/6/20.
//

#ifndef _ID3V1_H
#define _ID3V1_H

#include <cassert>

#include "Util.h"

NAME_SPACE_START(myUtil)

#define ID3V1_LEN 128

class ID3V1 {
public:
    void setValue(const uint8_t* data, size_t length) {
        assert(length == ID3V1_LEN);
        setValue(data, _tagFlag, 3);
        setValue(data + 3, _title, 30);
        setValue(data + 33, _artist, 30);
        setValue(data + 63, _album, 30);
        setValue(data + 93, _years, 4);
        setValue(data + 97, _comment, 30);
        _school = *(data + 127);
    }

protected:
    void setValue(const uint8_t* source, char* target, size_t len) {
        for (uint32_t i = 0; i < len; i++)
            target[i] = static_cast<char>(source[i]);
    }

public:
    char _tagFlag[3];
    char _title[30];
    char _artist[30];
    char _album[30];
    char _years[4];
    char _comment[30];
    uint8_t _school;
};

NAME_SPACE_END()

#endif // _ID3V1_H
