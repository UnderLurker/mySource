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

#define FULL_BOX_LEN   4
#define BOX_HEADER_LEN 8
#define USER_TYPE_LEN  16
#define CONVERT(a)     (((int32_t)(#a[0]) << 24) + ((int32_t)(#a[1]) << 16) + ((int32_t)(#a[2]) << 8) + (int32_t)(#a[3]))

enum BoxType : uint32_t {
    DINF = CONVERT(dinf),
    DREF = CONVERT(dref),
    FREE = CONVERT(free),
    FTYP = CONVERT(ftyp),
    HDLR = CONVERT(hdlr),
    MDAT = CONVERT(mdat),
    MDHD = CONVERT(mdhd),
    MDIA = CONVERT(mdia),
    MINF = CONVERT(minf),
    MOOF = CONVERT(moof),
    MOOV = CONVERT(moov),
    MVHD = CONVERT(mvhd),
    PDIN = CONVERT(pdin),
    SKIP = CONVERT(skip),
    STBL = CONVERT(stbl),
    STCO = CONVERT(stco),
    STSC = CONVERT(stsc),
    STSD = CONVERT(stsd),
    STTS = CONVERT(stts),
    TKHD = CONVERT(tkhd),
    TRAK = CONVERT(trak),
    TREF = CONVERT(tref),
    UUID = CONVERT(uuid),
    VMHD = CONVERT(vmhd),
};

struct BoxHeader {
    uint64_t size {0};
    BoxType type;
    uint8_t version;
    uint32_t flags;
    std::string TypeToString() const;
    void SetValue(const uint8_t* data, size_t length);
    void SetBigSize(const uint8_t* data, size_t length);
    uint64_t BodySize() const { return size > UINT32_MAX ? size - BOX_HEADER_LEN * 2 : size - BOX_HEADER_LEN; }
};

// Container
class Box {
public:
    using BoxPair = std::pair<BoxType, std::shared_ptr<Box>>;
    Box()         = default;
    virtual ~Box() { delete[] userType; }

    virtual MusicStatus ProcessHeader(std::fstream& file);
    // Analyze Box with SubBox
    virtual MusicStatus ProcessData(std::fstream& file);
    void ProcessFullBox(std::fstream& file);
    void ProcessFullBox(const uint8_t* body, size_t length);

public:
    BoxHeader _header {};
    std::list<BoxPair> boxes;
    uint8_t* userType {};
};

// Not container
class LeafBox : public virtual Box {
public:
    // Analyze Box
    MusicStatus ProcessData(std::fstream& file) override;

protected:
    /**
     * Assignment Data
     * @param body file data(exclude BoxHeader)
     * @param length
     * @return
 */
    virtual MusicStatus OnProcessData(const uint8_t* body, size_t length) { return SUCCESS; }
};

// File Type Box
struct FtypBox : public LeafBox {
    uint32_t majorBrand;
    uint32_t minorVersion;
    std::vector<uint32_t> compatibleBrands;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};

// Movie Header Box
struct MvhdBox : public LeafBox {
    uint64_t createTime;
    uint64_t modificationTime;
    uint32_t timeScale;
    uint64_t duration;
    uint32_t rate {0x00010000};
    int16_t volume {0x0100};
    uint8_t reserved[10] {0};
    int32_t matrix[9] {0x00010000, 0, 0, 0, 0x00010000, 0, 0, 0, 0x40000000};
    int32_t preDefined[6] {0};
    uint32_t nextTrackId;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};

struct MoovBox : public Box {};

// Media Data Box
struct MdatBox : public LeafBox {
    std::unique_ptr<uint8_t[]> data;
    size_t len {0};
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};

// Free Space Box (Can ignore)
struct FreeBox : public MdatBox {};
// Free Space Box (Can ignore)
struct SkipBox : public MdatBox {};

// Progressive Download Information Box
struct PdinBox : public LeafBox {
    struct PdinInfo {
        // a download rate expressed in bytes/second
        uint32_t rate;
        // InitialDelay is the suggested delay to use when playing the file, such that if download continues at the
        // given rate, all data within the file will arrive in time for its use and playback should not need to stall.
        uint32_t initialDelay;
    };
    std::list<PdinInfo> infos;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};

class M4a : public MusicBase {
public:
    explicit M4a(const char* filePath)
        : MusicBase(filePath) {
        _status = analysis();
    }
    MusicStatus analysis() override;

private:
    std::list<Box::BoxPair> _boxes;
};

} // namespace myUtil

#endif // _M4A_H
