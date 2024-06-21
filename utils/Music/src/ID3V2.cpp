//
// Created by XW on 2024/3/16.
//
#include "ID3V2.h"

#include <cmath>
#include <iostream>

namespace myUtil {
void convert16(uint16_t& value) {
    uint16_t value1 = value & 0x00ff;
    uint16_t value2 = value & 0xff00;
    value           = (value1 << 8) | (value2 >> 8);
}

void convert32(uint32_t& value) {
    uint32_t value1 = value & 0x000000ff;
    uint32_t value2 = value & 0x0000ff00;
    uint32_t value3 = value & 0x00ff0000;
    uint32_t value4 = value & 0xff000000;
    value           = (value1 << 24) | (value2 << 8) | (value3 >> 8) | (value4 >> 24);
}

uint32_t convertSize(uint32_t size) {
    return (size & 0xFF) + ((size >> 8) & 0xFF) * (1 << 2) + ((size >> 16) & 0xFF) * (1 << 4) +
           ((size >> 24) & 0xFF) * (1 << 8);
}

template<typename source_type, typename target_type>
target_type setByteValue(const source_type* data, size_t length, size_t bit = 8) {
    assert(data != nullptr);
    target_type result = 0;
    for (size_t i = 0; i < length; i++) {
        result = (result << bit) | (data[i] & ((uint32_t)pow(2, bit) - 1));
    }
    return result;
}

void TagHeader::setValue(const uint8_t* data, size_t length) {
    assert(length == 10);
    header   = setByteValue<uint8_t, uint32_t>(data, 3);
    ver      = *(data + 3);
    revision = *(data + 4);
    flag     = *(data + 5);
    size     = setByteValue<uint8_t, uint32_t>(data + 6, 4);
}

void ID3V2::readData(std::fstream& file) {
    std::unique_ptr<uint8_t[]> tagHeader = std::make_unique<uint8_t[]>(10);
    file.read((char*)tagHeader.get(), 10);
    _header.setValue(tagHeader.get(), 10);
    tagHeader.reset();
    uint32_t curPos = 0;
    while (curPos < _header.size) {
        uint8_t preview = file.get();
        file.seekg(-1, std::ios::cur);
        if (preview == 0) break;
        TagFrame frame = std::move(getTagFrame(file));
        _frame.push_back(frame);
        curPos += 10 + frame._header.size;
    }
    file.seekg(static_cast<uint32_t>(sizeof(TagHeader)) + _header.size, std::ios::beg);
}

TagFrame ID3V2::getTagFrame(std::fstream& file) {
    TagFrame frame;
    file.read((char*)&frame._header, sizeof(frame._header));
    convert32(frame._header.size);
    convert32(frame._header.id);
    frame._header.size = convertSize(frame._header.size);
    file.read((char*)&frame._flags, sizeof(uint16_t));
    convert16(frame._flags);
    frame._content = new int8_t[frame._header.size + 1];
    uint32_t size  = 0;
    while (file.get() == 0)
        size++;
    file.seekg(-1, std::ios::cur);
    for (int32_t i = 0; i < frame._header.size - size; i++) {
        frame._content[i] = static_cast<int8_t>(file.get());
    }
    frame._content[frame._header.size] = '\0';
    return frame;
}
} // namespace myUtil
