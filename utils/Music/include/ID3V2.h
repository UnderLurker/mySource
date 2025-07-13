//
// Created by XW on 2024/3/16.
//

#ifndef _ID3V2_H
#define _ID3V2_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <vector>

namespace myUtil {

#define IDV3 0x494433

struct TagHeader {
    uint32_t header {0};
    uint32_t size {0};
    uint8_t ver {0};
    uint8_t revision {0};
    uint8_t flag {0};
    [[nodiscard]] bool unsynchronisation() const { return (flag & 0x80) == 0x80; }
    [[nodiscard]] bool extension() const { return (flag & 0x40) == 0x40; }
    [[nodiscard]] bool test() const { return (flag & 0x40) == 0x40; }
    void setValue(const uint8_t* data, size_t length);
};

class TagFrame {
public:
    struct TagFrameHeader {
        uint32_t id;
        uint32_t size;
    };
    TagFrame() = default;
    TagFrame(const TagFrame& obj) {
        _header  = obj._header;
        _flags   = obj._flags;
        _content = new int8_t[obj._header.size + 1];
        memcpy(_content, obj._content, obj._header.size);
    }
    TagFrame(TagFrame&& obj) noexcept {
        _header      = obj._header;
        _flags       = obj._flags;
        _content     = obj._content;
        obj._content = nullptr;
    }
    ~TagFrame() {
        delete[] _content;
        _content = nullptr;
    }
    [[nodiscard]] bool tagProtect() const { return ((_flags >> 8) & 0x80) == 0x80; }
    [[nodiscard]] bool fileProtect() const { return ((_flags >> 8) & 0x40) == 0x40; }
    [[nodiscard]] bool readOnly() const { return ((_flags >> 8) & 0x20) == 0x20; }
    [[nodiscard]] bool compress() const { return ((_flags & 0xFF) & 0x80) == 0x80; }
    [[nodiscard]] bool encryption() const { return ((_flags & 0xFF) & 0x40) == 0x40; }
    [[nodiscard]] bool group() const { return ((_flags & 0xFF) & 0x20) == 0x20; }

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

private:
    TagFrame getTagFrame(std::fstream& file);

public:
    TagHeader _header {};
    std::vector<TagFrame> _frame;
};
} // namespace myUtil
#endif // _ID3V2_H
