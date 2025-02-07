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

#define BOX_HEADER_LEN 8
#define CONVERT(a)     (((int32_t)(#a[0]) << 24) + ((int32_t)(#a[1]) << 16) + ((int32_t)(#a[2]) << 8) + (int32_t)(#a[3]))

enum BoxType : uint32_t {
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
    MVHD = CONVERT(mvhd),
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

struct BoxHeader {
    uint64_t size {0};
    BoxType type;
    std::string TypeToString() const;
    void SetValue(const uint8_t* data, size_t length);
    void SetBigSize(const uint8_t* data, size_t length);
    uint64_t BodySize() const { return size > UINT32_MAX ? size - BOX_HEADER_LEN * 2 : size - BOX_HEADER_LEN; }
};

class Box {
public:
    Box()          = default;
    virtual ~Box() = default;

    virtual MusicStatus GetData(std::fstream& file) {
        try {
            auto data = new uint8_t[BOX_HEADER_LEN];
            file.read((char*)data, BOX_HEADER_LEN);
            _header.SetValue(data, BOX_HEADER_LEN);
            if (_header.size == 1) {
                file.read((char*)data, BOX_HEADER_LEN);
                _header.SetBigSize(data, BOX_HEADER_LEN);
            }
            delete[] data;
        } catch (...) { return ERROR_UNKNOWN; }
        return SUCCESS;
    }

public:
    BoxHeader _header {};
};
using BoxPair = std::pair<BoxType, std::shared_ptr<Box>>;

struct FtypBox : public Box {
    uint32_t majorBrand;
    uint32_t minorVersion;
    std::vector<uint32_t> compatibleBrands;
    MusicStatus GetData(std::fstream& file) override;
};

struct MvhdBox : public Box {
    uint8_t version;
    uint32_t flags: 24;
    uint32_t createTime;
    uint32_t modificationTime;
    uint32_t timeScale;
    uint32_t duration;
    uint32_t rate;
    uint16_t volume;
    uint8_t reserved[10];
    uint8_t matrix[36];
    uint8_t preDefined[24];
    uint32_t nextTrackId;
    MusicStatus GetData(std::fstream& file) override;
};

struct MoovBox : public Box {
    std::list<BoxPair> boxes;
    MusicStatus GetData(std::fstream& file) override;
};

class M4a : public MusicBase {
public:
    explicit M4a(const char* filePath)
        : MusicBase(filePath) {
        _status = analysis();
    }
    MusicStatus analysis() override;

private:
    std::list<BoxPair> _boxes;
};

} // namespace myUtil

#endif // _M4A_H
