//
// Created by XW on 2024/3/17.
//

#ifndef _MUSIC_FRAME_H
#define _MUSIC_FRAME_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>

#include "music_const.h"

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
    [[nodiscard]] MusicVersion version() const { return static_cast<MusicVersion>((_header.data2 & 0b00011000) >> 3); }
    [[nodiscard]] MusicLayer layer() const { return static_cast<MusicLayer>((_header.data2 & 0b00000110) >> 1); }
    [[nodiscard]] bool crc() const { return (_header.data2 & 0b00000001) == 0; }
    [[nodiscard]] uint8_t bitRateIndex() const { return _header.data3 >> 4; }
    [[nodiscard]] uint8_t samplingRateIndex() const { return (_header.data3 & 0b00001100) >> 2; }
    [[nodiscard]] bool padding() const { return (_header.data3 & 0b00000010) == 0b00000010; }
    [[nodiscard]] MusicChannel channel() const { return static_cast<MusicChannel>((_header.data4 & 0b11000000) >> 6); }
    [[nodiscard]] uint8_t extensionIndex() const { return (_header.data4 & 0b00110000) >> 4; }
    [[nodiscard]] bool copyright() const { return (_header.data4 & 0b00001000) == 0b00001000; }
    [[nodiscard]] bool original() const { return (_header.data4 & 0b00000100) == 0b00000100; }
    [[nodiscard]] MusicEmphasis emphasis() const { return static_cast<MusicEmphasis>(_header.data4 & 0b00000011); }
    [[nodiscard]] int16_t bitRate() const {
        return bitRateMap.at({
            {version(), layer()},
            bitRateIndex()
        });
    }
    [[nodiscard]] float samplingRate() const { return MusicSamplingRateMap.at({version(), samplingRateIndex()}); }
    [[nodiscard]] MusicExtension extension() const {
        assert(channel() == JOINT_STEREO);
        return MusicExtensionMap.at(extensionIndex());
    }
    [[nodiscard]] uint32_t frameSize() const {
        uint16_t sampleCount = MusicSamplingCountMap.at({version(), layer()});
        uint32_t fillSize    = 0;
        if (layer() == LAYER1) { fillSize = 0 * 4; }
        return ((sampleCount / 8 * (bitRate() * 1000)) / (uint32_t)(samplingRate() * 1000)) + fillSize;
    }
    [[nodiscard]] uint32_t time() const {
        uint16_t sampleCount = MusicSamplingCountMap.at({version(), layer()});
        return static_cast<uint32_t>((float)(sampleCount * 1000) / samplingRate());
    }

public:
    FrameHeader _header {};
};

class MusicFrameChannelInfo {
public:
    MusicFrameChannelInfo() = default;
    MusicFrameChannelInfo(const MusicFrameChannelInfo& obj) {
        _info = std::make_unique<uint8_t[]>(MUSIC_FRAME_CHANNEL_INFO_LEN);
        memcpy_s(_info.get(), MUSIC_FRAME_CHANNEL_INFO_LEN, obj._info.get(), MUSIC_FRAME_CHANNEL_INFO_LEN);
    }
    MusicFrameChannelInfo(MusicFrameChannelInfo&& obj) noexcept { _info = std::move(obj._info); }
    ~MusicFrameChannelInfo() = default;
    void readData(std::fstream& file);

public:
    std::unique_ptr<uint8_t[]> _info;
};

class MusicFrame {
public:
    MusicFrame()  = default;
    ~MusicFrame() = default;
    void readData(std::fstream& file);

public:
    MusicFrameHeader _header;
    uint16_t _crc {0};
    MusicFrameChannelInfo _channelInfo;
};

} // namespace myUtil

#endif // _MUSIC_FRAME_H
