//
// Created by CXN on 2025/2/6.
//

#ifndef _M4A_H
#define _M4A_H

#include <cstdint>
#include <fstream>
#include <list>
#include <map>

#include "music_base.h"
#include "Util.h"

namespace myUtil {

#define FULL_BOX_LEN   4
#define BOX_HEADER_LEN 8
#define USER_TYPE_LEN  16
#define CONVERT(a)     (((int32_t)(#a[0]) << 24) + ((int32_t)(#a[1]) << 16) + ((int32_t)(#a[2]) << 8) + (int32_t)(#a[3]))

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
    MINF = CONVERT(minf),
    MOOF = CONVERT(moof),
    MOOV = CONVERT(moov),
    MVHD = CONVERT(mvhd),
    NMHD = CONVERT(nmhd),
    PDIN = CONVERT(pdin),
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
    TKHD = CONVERT(tkhd),
    TRAK = CONVERT(trak),
    TREF = CONVERT(tref),
    TRGR = CONVERT(trgr),
    URL  = CONVERT(url),
    URN  = CONVERT(urn),
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

    virtual MusicStatus ProcessHeader(std::fstream& file);
    virtual MusicStatus ProcessHeader(const uint8_t* body, size_t length);
    // Analyze Box with SubBox
    virtual MusicStatus ProcessData(std::fstream& file);
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

class LeafFullBox : public virtual LeafBox, public virtual FullBox {};

///////////////////////////////////////////////////////////////////
///                         Other Boxes                         ///
///////////////////////////////////////////////////////////////////
struct FileTypeBox : public LeafBox {
    uint32_t majorBrand;
    uint32_t minorVersion;
    std::vector<uint32_t> compatibleBrands;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};

struct MovieHeaderBox : public LeafFullBox {
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

struct MovieBox : public Box {};

struct MediaDataBox : public LeafBox {
    std::unique_ptr<uint8_t[]> data;
    size_t len {0};
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};

// Free Space Box (Can ignore)
struct FreeBox : public MediaDataBox {};
// Free Space Box (Can ignore)
struct SkipBox : public MediaDataBox {};

struct ProgressiveDownloadInfoBox : public LeafBox {
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

struct TrackBox : public Box {};
struct TrackHeaderBox : public LeafFullBox {
    uint64_t creationTime;
    uint64_t modificationTime;
    uint32_t trackId;
    uint32_t reserved {0};
    uint64_t duration;
    int16_t layer {0};
    int16_t alternateGroup {0};
    int16_t volume; // if track_is_audio 0x0100 else 0
    int32_t matrix[9] {0x00010000, 0, 0, 0, 0x00010000, 0, 0, 0, 0x40000000};
    uint32_t width;
    uint32_t height;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
    [[nodiscard]] bool isAudio() const { return volume == 0x0100; }
};

struct TrackReferenceTypeBox : public LeafBox {
    std::unique_ptr<uint32_t[]> trackIDs;
    size_t len {0};
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};

struct TrackGroupTypeBox : public LeafBox {
    uint32_t trackGroupId;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};

struct MediaBox : public Box {};
struct MediaHeaderBox : public LeafFullBox {
    uint64_t creationTime;
    uint64_t modificationTime;
    uint32_t timeScale;
    uint64_t duration;
    bool pad {false};
    uint16_t language; // ISO-639-2/T language Code - Bit(15)
    uint16_t preDefined {0};
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct HandlerBox : public LeafFullBox {
    uint32_t preDefined {0};
    uint32_t handlerType;
    uint32_t reserved[3] {0};
    std::string name;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct MediaInformationBox : public Box {};
struct NullMediaHeaderBox : public FullBox {};
struct ExtendedLanguageBox : public LeafFullBox {
    std::string extendedLanguage;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct SampleTableBox : public Box {};
struct SampleDescriptionBox : public LeafFullBox {
    struct SampleEntry : public LeafBox {
        //    uint8_t reserved[6] = {0};
        uint16_t dataReferenceIndex;
    };
    uint32_t entryCount;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct DegradationPriorityBox : public LeafFullBox {
    std::unique_ptr<uint16_t[]> priorities;
    uint32_t len;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct TimeToSampleBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> sampleCounts;
    std::unique_ptr<uint32_t[]> sampleDelta;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct CompositionOffsetBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> sampleCount;
    std::unique_ptr<uint32_t[]> sampleOffsetU;
    std::unique_ptr<int32_t[]> sampleOffset;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct CompositionToDecodeBox : public LeafFullBox {
    int64_t compositionToDTSShift;
    int64_t leastDecodeToDisplayDelta;
    int64_t greatestDecodeToDisplayDelta;
    int64_t compositionStartTime;
    int64_t compositionEndTime;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct SyncSampleBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> sampleNumber;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct ShadowSyncSampleBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> shadowedSampleNumber;
    std::unique_ptr<uint32_t[]> syncSampleNumber;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct SampleDependencyTypeBox : public LeafFullBox {
    struct SampleDependencyTypeInfo {
        uint8_t isLeading           : 2;
        uint8_t sampleDependsOn     : 2;
        uint8_t sampleIsDependedOn  : 2;
        uint8_t sampleHasRedundancy : 2;
    };
    std::unique_ptr<SampleDependencyTypeInfo[]> infos;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct SampleSizeBox : public LeafFullBox {
    uint32_t sampleSize;
    uint32_t sampleCount;
    std::unique_ptr<uint32_t[]> entrySize;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct CompactSampleSizeBox : public LeafFullBox {
    uint8_t fieldSize;
    uint32_t sampleCount;
    std::unique_ptr<uint8_t[]> entrySize;
    // 未实现获取指定位置的entrySize
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct EditBox : public Box {};
struct EditListBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint64_t[]> segmentDuration64;
    std::unique_ptr<uint32_t[]> segmentDuration32;
    std::unique_ptr<int64_t[]> mediaTime64;
    std::unique_ptr<int32_t[]> mediaTime32;
    std::unique_ptr<int16_t[]> mediaRateInteger;
    std::unique_ptr<int16_t[]> mediaRateFraction;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct DataInformationBox : public Box {};
struct DataEntryUrlBox : public LeafFullBox { // type = 'url '
    std::string location;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct DataEntryUrnBox : public LeafFullBox { // type = 'urn '
    std::string name;
    std::string location;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct DataReferenceBox : public FullBox {
    MusicStatus ProcessData(std::fstream& file) override;
};
struct SampleToChunkBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> firstChunk;
    std::unique_ptr<uint32_t[]> samplesPerChunk;
    std::unique_ptr<uint32_t[]> sampleDescriptionIndex;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct ChunkOffsetBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint32_t[]> chunkOffset;
    MusicStatus OnProcessData(const uint8_t* body, size_t length) override;
};
struct ChunkLargeOffsetBox : public LeafFullBox {
    uint32_t entryCount;
    std::unique_ptr<uint64_t[]> chunkOffset;
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
    Box::SubBoxMap _boxes;
};

} // namespace myUtil

#endif // _M4A_H
