//
// Created by 常笑男 on 2025/4/4.
//

#ifndef _M4A_COMMON_H
#define _M4A_COMMON_H

#include <cstdint>
#include <fstream>
#include <list>
#include <memory>

#include "m4a/m4a_const.h"
#include "Util.h"

namespace myUtil {

#define FULL_BOX_LEN   4
#define BOX_HEADER_LEN 8
#define USER_TYPE_LEN  16
#define CONVERT(a)     (((int32_t)(#a[0]) << 24) + ((int32_t)(#a[1]) << 16) + ((int32_t)(#a[2]) << 8) + (int32_t)(#a[3]))
#define STRING_FILL    ' '
enum BoxType : uint32_t {
    CO64 = CONVERT(co64),
    CSLG = CONVERT(cslg),
    CTTS = CONVERT(ctts),
    DINF = CONVERT(dinf),
    DREF = CONVERT(dref),
    EDTS = CONVERT(edts),
    ELNG = CONVERT(elng),
    ELST = CONVERT(elst),
    FREE = CONVERT(free),
    FTYP = CONVERT(ftyp),
    HDLR = CONVERT(hdlr),
    MDAT = CONVERT(mdat),
    MDHD = CONVERT(mdhd),
    MDIA = CONVERT(mdia),
    MEHD = CONVERT(mehd),
    MFHD = CONVERT(mfhd),
    MFRA = CONVERT(mfra),
    MINF = CONVERT(minf),
    MOOF = CONVERT(moof),
    MOOV = CONVERT(moov),
    MVEX = CONVERT(mvex),
    MVHD = CONVERT(mvhd),
    NMHD = CONVERT(nmhd),
    PADB = CONVERT(padb),
    PDIN = CONVERT(pdin),
    SAIO = CONVERT(saio),
    SAIZ = CONVERT(saiz),
    SDTP = CONVERT(sdtp),
    SKIP = CONVERT(skip),
    STBL = CONVERT(stbl),
    STCO = CONVERT(stco),
    STDP = CONVERT(stdp),
    STSC = CONVERT(stsc),
    STSD = CONVERT(stsd),
    STSH = CONVERT(stsh),
    STSS = CONVERT(stss),
    STSZ = CONVERT(stsz),
    STTS = CONVERT(stts),
    STZ2 = CONVERT(stz2),
    SUBS = CONVERT(subs),
    TFHD = CONVERT(tfhd),
    TFRA = CONVERT(tfra),
    TKHD = CONVERT(tkhd),
    TRAF = CONVERT(traf),
    TRAK = CONVERT(trak),
    TREF = CONVERT(tref),
    TREX = CONVERT(trex),
    TRGR = CONVERT(trgr),
    TRUN = CONVERT(trun),
    URL  = CONVERT(url ),
    URN  = CONVERT(urn ),
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
///////////////////////////////////////////////////////////////////
///                          framework                          ///
///////////////////////////////////////////////////////////////////
// Container
class Box {
public:
    using BoxList   = std::list<std::shared_ptr<Box>>;
    using SubBoxMap = std::map<BoxType, BoxList>;
    Box()           = default;
    virtual ~Box() { delete[] _userType; }

    virtual M4AStatus ProcessHeader(std::fstream& file);
    virtual M4AStatus ProcessHeader(const uint8_t* body, size_t length);
    // Analyze Box with SubBox
    virtual M4AStatus ProcessData(std::fstream& file);
    virtual bool ProcessFullBox(std::fstream& file) { return false; }
    virtual bool ProcessFullBox(const uint8_t* body, size_t length) { return false; }

    uint32_t Count(BoxType type) const;
    uint32_t ParentCount(BoxType type) const;
    BoxList GetBoxList(BoxType type);

public:
    BoxHeader _header {};
    SubBoxMap _boxes;
    uint8_t* _userType {};
    std::weak_ptr<Box> _parent;
};

// Container
class FullBox : public virtual Box {
public:
    bool ProcessFullBox(std::fstream& file) override;
    bool ProcessFullBox(const uint8_t* body, size_t length) override;
};

// Not container
class LeafBox : public virtual Box {
public:
    // Analyze Box
    M4AStatus ProcessData(std::fstream& file) override;

protected:
    /**
     * Assignment Data
     * @param body file data(exclude BoxHeader)
     * @param length
     * @return
     */
    virtual M4AStatus OnProcessData(const uint8_t* body, size_t length) { return SUCCESS; }
};

class LeafFullBox : public virtual LeafBox, public virtual FullBox {};

// FTYP
struct FileTypeBox : public LeafBox {
    uint32_t majorBrand;
    uint32_t minorVersion;
    std::vector<uint32_t> compatibleBrands;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};

template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
T GetValue(const uint8_t* data) {
    T result = 0;
    for (uint32_t i = 0; i < sizeof(T); i++) {
        result = (result << 8) | data[i];
    }
    return result;
};

template<typename T>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template<typename T, std::enable_if_t<is_optional_v<T>, bool> = true>
void ReadData(const uint8_t* body, T& data, uint32_t& pos) {
    data  = GetValue<typename T::value_type>(body + pos);
    pos  += sizeof(typename T::value_type);
}
} // namespace myUtil

#endif // _M4A_COMMON_H
