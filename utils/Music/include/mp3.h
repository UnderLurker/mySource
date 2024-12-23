#pragma once

#ifndef _MUSIC_H
#define _MUSIC_H

#include <utility>

#include "ID3V1.h"
#include "ID3V2.h"
#include "music_base.h"
#include "music_frame.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

class MP3 : public MusicBase {
public:
    MP3() = default;
    explicit MP3(const char* filePath)
        : MusicBase(filePath) {
        _status = analysis();
    }
    MusicStatus analysis() override;

public:
    ID3V1 _id3v1;
    ID3V2 _id3v2;
    vector<MusicFrame> _frames;
};

NAME_SPACE_END()

#endif //!_MUSIC_H
