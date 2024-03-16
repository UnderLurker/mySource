//
// Created by XW on 2024/3/16.
//

#ifndef _ID3V2_H
#define _ID3V2_H

#include <cstdint>
#include <fstream>
#include <memory>
#include <cstring>
#include <vector>

namespace myUtil {

#define IDV3 0x494433

struct TagHeader {
    uint8_t header1;
    uint8_t header2;
    uint8_t header3;
    uint8_t ver;
    uint8_t revision;
    uint8_t flag;
    uint8_t size1;
    uint8_t size2;
    uint8_t size3;
    uint8_t size4;
    [[nodiscard]] uint32_t size() const {
        return (size1 & 0x7F) * (1 << (7 * 3)) + (size2 & 0x7F) * (1 << (7 * 2)) + (size3 & 0x7F) * (1 << (7 * 1)) +
               (size4 & 0x7F);
    }
    [[nodiscard]] uint32_t header() const { return (((header1 << 8) + header2) << 8) + header3; }
};

class TagFrame {
public:
    struct TagFrameHeader {
        uint32_t id;
        uint32_t size;
    };
    TagFrame() = default;
    TagFrame(const TagFrame& obj) {
        _header = obj._header;
        _flags = obj._flags;
        _content = new int8_t[obj._header.size + 1];
        memcpy_s(_content, obj._header.size, obj._content, obj._header.size);
    }
    ~TagFrame() {
        delete[] _content;
        _content = nullptr;
    }

public:
    TagFrameHeader _header {};
    uint16_t _flags {};
    int8_t* _content {nullptr};
};

class ID3V2 {
public:
    ID3V2()  = default;
    ~ID3V2() = default;
    void readData(std::fstream& file);

public:
    TagHeader _header {};
    std::vector<TagFrame> _frame;
};
} // namespace myUtil
#endif // _ID3V2_H
