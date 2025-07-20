//
// Created by XW on 2024/3/17.
//
#include "music_frame.h"

#include <iostream>

#include "Util.h"
#include "include/logger_wrapper.h"

namespace myUtil {

MusicExtension MusicFrameHeader::extension() const {
    MYASSERT(channel() == JOINT_STEREO);
    return MusicExtensionMap.at(extensionIndex());
}

int16_t MusicFrameHeader::bitRate() const {
    return bitRateMap.at({
        {version(), layer()},
        bitRateIndex()
    });
}

uint32_t MusicFrameHeader::frameLength() const {
    uint16_t sampleCount = frameSize();
    uint32_t fillSize    = 1;
    if (layer() == LAYER1) fillSize = 4;
    return ((sampleCount / 8 * (bitRate() * 1000)) / (uint32_t)(samplingRate() * 1000)) + fillSize * padding();
}

uint32_t MusicFrameHeader::frameTime() const {
    return static_cast<uint32_t>((float)(frameSize() * 1000) / samplingRate());
}

void MusicFrameChannelInfo::readData(const uint8_t* info, size_t& length) {
    _info = std::make_unique<uint8_t[]>(MUSIC_FRAME_CHANNEL_INFO_LEN);
    for (int32_t i = 0; i < MUSIC_FRAME_CHANNEL_INFO_LEN; i++) {
        _info[i] = info[length++];
    }
}

bool MusicFrame::readData(std::fstream& file) {
#ifdef UTIL_DEBUG
    std::stringstream ss;
    ss << std::hex << file.tellg();
    LOGD("tellg : 0x%s", ss.str().c_str());
#endif
    constexpr uint32_t frameHeaderSize = 4;
    file.read((char*)&_header, frameHeaderSize);
    if (_header.sync() != 0x7FF) return false;
    // 存储帧数据
    uint32_t length = _header.frameLength();
    LOGD("curLength: %d", length);
    auto frameInfo = std::make_unique<uint8_t[]>(length - frameHeaderSize);
    file.read((char*)frameInfo.get(), length - frameHeaderSize);
    size_t offset = 0;
    if (_header.crc()) {
        _crc    = (frameInfo[offset] << 8) + frameInfo[offset + 1];
        offset += 2;
    }
    _channelInfo.readData(frameInfo.get(), offset);
    return true;
}
} // namespace myUtil
