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
    [[nodiscard]] uint8_t padding() const { return (_header.data3 & 0b00000010) >> 1; }
    [[nodiscard]] MusicChannel channel() const { return static_cast<MusicChannel>((_header.data4 & 0b11000000) >> 6); }
    [[nodiscard]] uint8_t extensionIndex() const { return (_header.data4 & 0b00110000) >> 4; }
    [[nodiscard]] bool copyright() const { return (_header.data4 & 0b00001000) == 0b00001000; }
    [[nodiscard]] bool original() const { return (_header.data4 & 0b00000100) == 0b00000100; }
    [[nodiscard]] MusicEmphasis emphasis() const { return static_cast<MusicEmphasis>(_header.data4 & 0b00000011); }
    [[nodiscard]] float samplingRate() const { return MusicSamplingRateMap.at({version(), samplingRateIndex()}); }
    /**
     * 帧大小，即每帧采样数
     * @return
     */
    [[nodiscard]] uint16_t frameSize() const { return MusicSamplingCountMap.at({version(), layer()}); }
    [[nodiscard]] MusicExtension extension() const;
    [[nodiscard]] int16_t bitRate() const;
    /**
     * 帧长度，不同版本的长度不同
     * @return
     */
    [[nodiscard]] uint32_t frameLength() const;
    /**
     * 每帧持续时间
     * @return 保留整数
     */
    [[nodiscard]] uint32_t frameTime() const;

public:
    FrameHeader _header {};
};

class MusicFrameChannelInfo {
public:
    MusicFrameChannelInfo() = default;
    MusicFrameChannelInfo(const MusicFrameChannelInfo& obj) {
        _info = std::make_unique<uint8_t[]>(MUSIC_FRAME_CHANNEL_INFO_LEN);
        memcpy(_info.get(), obj._info.get(), MUSIC_FRAME_CHANNEL_INFO_LEN);
    }
    MusicFrameChannelInfo(MusicFrameChannelInfo&& obj) noexcept { _info = std::move(obj._info); }
    ~MusicFrameChannelInfo() = default;
    void readData(const uint8_t* info, size_t& length);

public:
    std::unique_ptr<uint8_t[]> _info;
};

class MusicFrame {
public:
    MusicFrame()  = default;
    ~MusicFrame() = default;
    // return true 表示当前帧是正确的，返回false表明当前不是数据帧
    bool readData(std::fstream& file);

public:
    MusicFrameHeader _header;
    uint16_t _crc {0};
    MusicFrameChannelInfo _channelInfo;
};

} // namespace myUtil

#endif // _MUSIC_FRAME_H
