//
// Created by XW on 2024/3/17.
//
#include "music_frame.h"
#include <iostream>

namespace myUtil {

MusicExtension MusicFrameHeader::extension() const {
    assert(channel() == JOINT_STEREO);
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

void MusicFrameChannelInfo::readData(std::fstream& file) {
    _info = std::make_unique<uint8_t[]>(MUSIC_FRAME_CHANNEL_INFO_LEN);
    for (int32_t i = 0; i < MUSIC_FRAME_CHANNEL_INFO_LEN; i++) {
        _info[i] = file.get();
    }
}

void MusicFrame::readData(std::fstream& file) {
    file.read((char*)&_header, 4);
    std::cout << std::hex << file.tellg() << std::endl;
    if (_header.crc()) { _crc = (file.get() << 8) + file.get(); }
    _channelInfo.readData(file);
    uint32_t size = _header.frameSize();
}
} // namespace myUtil
