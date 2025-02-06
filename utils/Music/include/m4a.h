//
// Created by CXN on 2025/2/6.
//

#ifndef _M4A_H
#define _M4A_H

#include <cstdint>
#include <fstream>
#include <list>

#include "music_base.h"
#include "Util.h"

namespace myUtil {

#define BOX_HEADER_LEN 16
#define CONVERT(a)     (((int32_t)(#a[0]) << 24) + ((int32_t)(#a[1]) << 16) + ((int32_t)(#a[2]) << 8) + (int32_t)(#a[3]))

enum BoxType {
    DINF = CONVERT(dinf),
    DREF = CONVERT(dref),
    FTYP = CONVERT(ftyp),
    HDLR = CONVERT(hdlr),
    MDAT = CONVERT(mdat),
    MDHD = CONVERT(mdhd),
    MDIA = CONVERT(mdia),
    MINF = CONVERT(minf),
    MOOF = CONVERT(moof),
    MOOV = CONVERT(moov),
    STBL = CONVERT(stbl),
    STCO = CONVERT(stco),
    STSC = CONVERT(stsc),
    STSD = CONVERT(stsd),
    STTS = CONVERT(stts),
    TKHD = CONVERT(tkhd),
    TRAK = CONVERT(trak),
    TREF = CONVERT(tref),
    VMHD = CONVERT(vmhd),
};

class BoxHeader {
public:
    [[nodiscard]] uint64_t Size() const { return _size == 1 ? _largeSize : _size; }
    [[nodiscard]] BoxType Type() const { return (BoxType)_type; }
    [[nodiscard]] std::string TypeToString() const;
    void ReadData(const uint8_t* data, size_t length);

private:
    uint32_t _size;
    uint32_t _type;
    uint64_t _largeSize;
};

class Box {
public:
    Box()          = default;
    virtual ~Box() = default;

    virtual MusicStatus GetData(std::fstream& file) {
        try {
            auto data = new uint8_t[BOX_HEADER_LEN];
            file.read((char*)data, BOX_HEADER_LEN);
            _header.ReadData(data, BOX_HEADER_LEN);
            delete[] data;
        } catch (...) { return ERROR_UNKNOWN; }
        return SUCCESS;
    }

public:
    BoxHeader _header {};
};

struct FtypBox : public Box {
    MusicStatus GetData(std::fstream& file) override;
    uint32_t majorBrand;
    uint32_t minorVersion;
    std::vector<uint32_t> compatibleBrands;
};

class M4a : public MusicBase {
    using BoxPair = std::pair<BoxType, std::shared_ptr<Box>>;

public:
    explicit M4a(const char* filePath)
        : MusicBase(filePath) {
        _status = analysis();
    }
    MusicStatus analysis() override;

protected:
    MusicStatus ProcessBox();

private:
    std::list<BoxPair> _boxes;
};

} // namespace myUtil

#endif // _M4A_H
