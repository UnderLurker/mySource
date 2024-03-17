//
// Created by XW on 2024/3/17.
//

#ifndef _MUSIC_FRAME_H
#define _MUSIC_FRAME_H

#include <cstdint>
#include <cstring>
#include <fstream>

namespace myUtil {
#define MUSIC_FRAME_CHANNEL_INFO_LEN 32
class MusicFrameHeader {
public:
    struct FrameHeader {
        uint8_t data1;
        uint8_t data2;
        uint8_t data3;
        uint8_t data4;
    };
    MusicFrameHeader()  = default;
    ~MusicFrameHeader() = default;
    [[nodiscard]] uint16_t sync() const { return (_header.data1 << 3) + (_header.data2 >> 5); }
    [[nodiscard]] uint8_t version() const { return (_header.data2 & 0b00011000) >> 3; }
    [[nodiscard]] uint8_t layer() const { return (_header.data2 & 0b00000110) >> 1; }
    [[nodiscard]] bool crc() const { return (_header.data2 & 0b00000001) == 0; }
    [[nodiscard]] uint8_t bitRateIndex() const { return _header.data3 >> 4; }
    [[nodiscard]] uint8_t samplingFrequency() const { return (_header.data3 & 0b00001100) >> 2; }
    [[nodiscard]] bool padding() const { return (_header.data3 & 0b00000010) == 0b00000010; }
    [[nodiscard]] uint8_t mode() const { return (_header.data4 & 0b11000000) >> 6; }
    [[nodiscard]] uint8_t extension() const { return (_header.data4 & 0b00110000) >> 4; }
    [[nodiscard]] bool copyright() const { return (_header.data4 & 0b00001000) == 0b00001000; }
    [[nodiscard]] bool original() const { return (_header.data4 & 0b00000100) == 0b00000100; }
    [[nodiscard]] uint8_t emphasis() const { return _header.data4 & 0b00000011; }

public:
    FrameHeader _header {};
};

class MusicFrameChannelInfo {
public:
    MusicFrameChannelInfo() = default;
    MusicFrameChannelInfo(const MusicFrameChannelInfo& obj) {
        delete[] _info;
        _info = new uint8_t[MUSIC_FRAME_CHANNEL_INFO_LEN];
        memcpy_s(_info, MUSIC_FRAME_CHANNEL_INFO_LEN, obj._info, MUSIC_FRAME_CHANNEL_INFO_LEN);
    }
    MusicFrameChannelInfo(MusicFrameChannelInfo&& obj)  noexcept {
        delete[] _info;
        _info = obj._info;
        obj._info = nullptr;
    }
    ~MusicFrameChannelInfo() {
        delete[] _info;
        _info = nullptr;
    }
    void readData(std::fstream& file);
public:
    uint8_t* _info{nullptr};
};

class MusicFrame {
public:
    MusicFrame()  = default;
    ~MusicFrame() = default;
    void readData(std::fstream& file);

public:
    MusicFrameHeader _header;
    uint16_t _crc{0};
    MusicFrameChannelInfo _channelInfo;
};

} // namespace myUtil

#endif // _MUSIC_FRAME_H
