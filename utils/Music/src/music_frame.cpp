//
// Created by XW on 2024/3/17.
//
#include "music_frame.h"

namespace myUtil {
void MusicFrameChannelInfo::readData(std::fstream& file) {
    _info = new uint8_t[MUSIC_FRAME_CHANNEL_INFO_LEN];
    for (int32_t i = 0; i < MUSIC_FRAME_CHANNEL_INFO_LEN; i++) {
        _info[i] = file.get();
    }
}

void MusicFrame::readData(std::fstream& file) {
    file.read((char*)&_header, 4);
    if (_header.crc()) { _crc = (file.get() << 8) + file.get(); }
    _channelInfo.readData(file);
}
} // namespace myUtil
