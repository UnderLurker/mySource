//
// Created by CXN on 2025/2/6.
//
#include "m4a.h"

namespace myUtil {
namespace {
constexpr char STRING_FILL = ' ';
template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
T GetValue(const uint8_t* data) {
    T result = 0;
    for (uint32_t i = 0; i < sizeof(T); i++) {
        result = (result << 8) | data[i];
    }
    return result;
};

std::map<BoxType, std::function<std::shared_ptr<Box>()>> BoxMap = {
    {myUtil::BoxType::FTYP, []() { return std::make_shared<FileTypeBox>(); }               },
    {myUtil::BoxType::MOOV, []() { return std::make_shared<MovieBox>(); }                  },
    {myUtil::BoxType::MVHD, []() { return std::make_shared<MovieHeaderBox>(); }            },
    {myUtil::BoxType::MDAT, []() { return std::make_shared<MediaDataBox>(); }              },
    {myUtil::BoxType::FREE, []() { return std::make_shared<FreeBox>(); }                   },
    {myUtil::BoxType::SKIP, []() { return std::make_shared<SkipBox>(); }                   },
    {myUtil::BoxType::PDIN, []() { return std::make_shared<ProgressiveDownloadInfoBox>(); }},
    {myUtil::BoxType::TRAK, []() { return std::make_shared<TrackBox>(); }                  },
    {myUtil::BoxType::TKHD, []() { return std::make_shared<TrackHeaderBox>(); }            },
    {myUtil::BoxType::TREF, []() { return std::make_shared<TrackReferenceTypeBox>(); }     },
    {myUtil::BoxType::TRGR, []() { return std::make_shared<TrackGroupTypeBox>(); }         },
    {myUtil::BoxType::MDHD, []() { return std::make_shared<MediaHeaderBox>(); }            },
    {myUtil::BoxType::HDLR, []() { return std::make_shared<HandlerBox>(); }                },
    {myUtil::BoxType::MINF, []() { return std::make_shared<MediaInformationBox>(); }       },
    {myUtil::BoxType::NMHD, []() { return std::make_shared<NullMediaHeaderBox>(); }        },
    {myUtil::BoxType::ELNG, []() { return std::make_shared<ExtendedLanguageBox>(); }       },
    {myUtil::BoxType::STBL, []() { return std::make_shared<SampleTableBox>(); }            },
    {myUtil::BoxType::STSD, []() { return std::make_shared<SampleDescriptionBox>(); }      },
    {myUtil::BoxType::STDP, []() { return std::make_shared<DegradationPriorityBox>(); }    },
    {myUtil::BoxType::STTS, []() { return std::make_shared<TimeToSampleBox>(); }           },
    {myUtil::BoxType::CTTS, []() { return std::make_shared<CompositionOffsetBox>(); }      },
    {myUtil::BoxType::CSLG, []() { return std::make_shared<CompositionToDecodeBox>(); }    },
    {myUtil::BoxType::STSS, []() { return std::make_shared<SyncSampleBox>(); }             },
    {myUtil::BoxType::STSH, []() { return std::make_shared<ShadowSyncSampleBox>(); }       },
    {myUtil::BoxType::SDTP, []() { return std::make_shared<SampleDependencyTypeBox>(); }   },
    {myUtil::BoxType::STSZ, []() { return std::make_shared<SampleSizeBox>(); }             },
    {myUtil::BoxType::STZ2, []() { return std::make_shared<CompactSampleSizeBox>(); }      },
    {myUtil::BoxType::EDTS, []() { return std::make_shared<EditBox>(); }                   },
    {myUtil::BoxType::ELST, []() { return std::make_shared<EditListBox>(); }               },
};
} // namespace

std::string BoxHeader::TypeToString() const {
    std::string res;
    auto tmp = (uint32_t)type;
    while (tmp) {
        res.append({(char)(tmp & 0xFF)});
        tmp >>= 8;
    }
    std::reverse(res.begin(), res.end());
    return res;
}

void BoxHeader::SetValue(const uint8_t* data, size_t length) {
    MYASSERT(length == BOX_HEADER_LEN)
    size = GetValue<uint32_t>(data);
    type = static_cast<BoxType>(GetValue<uint32_t>(data + 4));
}

void BoxHeader::SetBigSize(const uint8_t* data, size_t length) {
    MYASSERT(length == BOX_HEADER_LEN)
    size = GetValue<uint64_t>(data);
}

///////////////////////////////////////////////////////////////////
///                          framework                          ///
///////////////////////////////////////////////////////////////////
MusicStatus Box::ProcessHeader(std::fstream& file) {
    try {
        auto data = new uint8_t[BOX_HEADER_LEN];
        file.read((char*)data, BOX_HEADER_LEN);
        _header.SetValue(data, BOX_HEADER_LEN);
        if (_header.size == 1) {
            file.read((char*)data, BOX_HEADER_LEN);
            _header.SetBigSize(data, BOX_HEADER_LEN);
        }
        if (_header.type == myUtil::BoxType::UUID) {
            _userType = new uint8_t[USER_TYPE_LEN];
            file.read((char*)_userType, 16);
        }
        delete[] data;
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

MusicStatus Box::ProcessData(std::fstream& file) {
    int64_t remainLen = _header.BodySize();
    if (remainLen < 8) return ERROR_FILE_FORMAT;
    if (ProcessFullBox(file)) remainLen -= FULL_BOX_LEN;
    while (remainLen > 0) {
#ifdef UTIL_DEBUG
        std::stringstream ss;
        ss << file.tellg();
        uint32_t pos = 0;
        ss >> pos;
#endif
        Box box;
        box.ProcessHeader(file);
#ifdef UTIL_DEBUG
        LOGI(" Box SIZE:0x%x, \ttype:%s, pos:0x%x", box._header.size, box._header.TypeToString().c_str(), pos);
#endif
        auto bodySize = box._header.BodySize();
        auto iter     = BoxMap.find(box._header.type);
        if (iter == BoxMap.end()) {
            file.seekg(bodySize, std::ios::cur);
        } else {
            auto subBox     = iter->second();
            subBox->_header = box._header;
            //            subBox->_parent = std::weak_ptr(this);
            subBox->ProcessData(file);
            auto mapIter = _boxes.find(subBox->_header.type);
            if (mapIter == _boxes.end()) {
                _boxes[subBox->_header.type] = {subBox};
            } else {
                _boxes[subBox->_header.type].emplace_back(subBox);
            }
        }
        remainLen -= bodySize;
    }
    return SUCCESS;
}

uint32_t Box::Count(BoxType type) const { return _boxes.count(type); }

uint32_t Box::ParentCount(BoxType type) const {
    std::shared_ptr<Box> parent(_parent);
    if (!parent) return 0;
    return parent->Count(type);
}

Box::BoxList Box::GetBoxList(BoxType type) {
    if (Count(type)) return _boxes[type];
    return {};
}

bool FullBox::ProcessFullBox(std::fstream& file) {
    auto data = new int8_t[4];
    file.read((char*)data, 4);
    ProcessFullBox(reinterpret_cast<const uint8_t*>(data), 4);
    delete[] data;
    return true;
}
bool FullBox::ProcessFullBox(const uint8_t* body, size_t length) {
    _header.version = body[0];
    _header.flags   = (GetValue<uint8_t>(body + 1) << 16) | GetValue<uint16_t>(body + 2);
    return true;
}

MusicStatus LeafBox::ProcessData(std::fstream& file) {
    auto remainLen = _header.BodySize();
    if (remainLen < 8) return ERROR_FILE_FORMAT;
    auto body = new uint8_t[remainLen];
    file.read((char*)body, remainLen);
    MusicStatus ret;
    if (ProcessFullBox(body, FULL_BOX_LEN)) {
        ret = OnProcessData(body + FULL_BOX_LEN, remainLen - FULL_BOX_LEN);
    } else {
        ret = OnProcessData(body, remainLen);
    }
    delete[] body;
    return ret;
}

///////////////////////////////////////////////////////////////////
///                         Other Boxes                         ///
///////////////////////////////////////////////////////////////////
MusicStatus FileTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    majorBrand    = GetValue<uint32_t>(body);
    minorVersion  = GetValue<uint32_t>(body + 4);
    length       -= 8;
    auto start    = 8;
    while (length > 0) {
        compatibleBrands.push_back(GetValue<uint32_t>(body + start));
        length -= sizeof(uint32_t);
        start  += sizeof(uint32_t);
    }
    return SUCCESS;
}

MusicStatus MovieHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    if ((_header.version == 1 && length != 108) || (_header.version == 0 && length != 96)) return ERROR_FILE_FORMAT;
    auto pos = 0;
    if (_header.version == 1) {
        createTime       = GetValue<uint64_t>(body);
        modificationTime = GetValue<uint64_t>(body + 8);
        timeScale        = GetValue<uint32_t>(body + 16);
        duration         = GetValue<uint64_t>(body + 20);
        pos              = 28;
    } else { // version == 0
        createTime       = GetValue<uint32_t>(body);
        modificationTime = GetValue<uint32_t>(body + 4);
        timeScale        = GetValue<uint32_t>(body + 8);
        duration         = GetValue<uint32_t>(body + 12);
        pos              = 16;
    }
    rate        = GetValue<uint32_t>(body + pos);
    volume      = GetValue<int16_t>(body + pos + 4);
    nextTrackId = GetValue<uint32_t>(body + pos + 76);
    memcpy(reserved, body + pos + 6, 10);
    memcpy(matrix, body + pos + 16, 36);
    for (size_t i = 0; i < sizeof(matrix) / sizeof(int32_t); i++) {
        auto tmp  = body + pos + 16 + 4 * i;
        matrix[i] = GetValue<int32_t>(tmp);
    }
    for (size_t i = 0; i < sizeof(preDefined) / sizeof(int32_t); i++) {
        auto tmp      = body + pos + 52 + 4 * i;
        preDefined[i] = GetValue<int32_t>(tmp);
    }
    return SUCCESS;
}

MusicStatus MediaDataBox::OnProcessData(const uint8_t* body, size_t length) {
    data = std::make_unique<uint8_t[]>(length);
    memcpy(data.get(), body, length);
    len = length;
    return SUCCESS;
}

MusicStatus ProgressiveDownloadInfoBox::OnProcessData(const uint8_t* body, size_t length) {
    size_t size = sizeof(PdinInfo);
    if (length % size != 0) return ERROR_FILE_FORMAT;
    size_t l = 0 + FULL_BOX_LEN, r = 4 + FULL_BOX_LEN;
    while (length) {
        infos.push_back({GetValue<uint32_t>(body + l), GetValue<uint32_t>(body + l + 4)});
        length -= size;
        l      += size;
        r      += size;
    }
    return SUCCESS;
}

MusicStatus TrackHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    const uint8_t* tmpBody = body;
    if (_header.version == 1) {
        creationTime     = GetValue<uint64_t>(tmpBody);
        modificationTime = GetValue<uint64_t>(tmpBody + 8);
        trackId          = GetValue<uint32_t>(tmpBody + 16);
        reserved         = GetValue<uint32_t>(tmpBody + 20);
        duration         = GetValue<uint64_t>(tmpBody + 24);
        tmpBody          = tmpBody + 32;
    } else { // version == 0
        creationTime     = GetValue<uint32_t>(tmpBody);
        modificationTime = GetValue<uint32_t>(tmpBody + 4);
        trackId          = GetValue<uint32_t>(tmpBody + 8);
        reserved         = GetValue<uint32_t>(tmpBody + 12);
        duration         = GetValue<uint32_t>(tmpBody + 16);
        tmpBody          = tmpBody + 20;
    }
    tmpBody        = tmpBody + 8;
    layer          = GetValue<int16_t>(tmpBody);
    alternateGroup = GetValue<int16_t>(tmpBody + 2);
    volume         = GetValue<int16_t>(tmpBody + 4);
    width          = GetValue<uint32_t>(tmpBody + 44);
    height         = GetValue<uint32_t>(tmpBody + 48);
    for (size_t i = 0; i < sizeof(matrix) / sizeof(int32_t); i++) {
        auto tmp  = tmpBody + 8 + 4 * i;
        matrix[i] = GetValue<int32_t>(tmp);
    }
    return SUCCESS;
}

MusicStatus TrackReferenceTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length % 4 != 0) return ERROR_FILE_FORMAT;
    len      = length / 4;
    trackIDs = std::make_unique<uint32_t[]>(len);
    for (size_t i = 0; i < length; i += 4) {
        trackIDs[i] = GetValue<uint32_t>(body + i);
    }
    return SUCCESS;
}

MusicStatus TrackGroupTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length != sizeof(trackGroupId)) return ERROR_FILE_FORMAT;
    trackGroupId = GetValue<uint32_t>(body);
    return SUCCESS;
}

MusicStatus MediaHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    auto tmpBody = body;
    if (_header.version == 1) {
        creationTime      = GetValue<uint64_t>(tmpBody);
        modificationTime  = GetValue<uint64_t>(tmpBody + 8);
        timeScale         = GetValue<uint32_t>(tmpBody + 16);
        duration          = GetValue<uint64_t>(tmpBody + 20);
        tmpBody          += 28;
    } else { // version == 0;
        creationTime      = GetValue<uint32_t>(tmpBody);
        modificationTime  = GetValue<uint32_t>(tmpBody + 4);
        timeScale         = GetValue<uint32_t>(tmpBody + 8);
        duration          = GetValue<uint32_t>(tmpBody + 12);
        tmpBody          += 16;
    }
    pad        = (tmpBody[0] & 0x80) == 0x80;
    language   = GetValue<uint16_t>(tmpBody) | 0x7F;
    preDefined = GetValue<uint16_t>(tmpBody + 2);
    return SUCCESS;
}

MusicStatus HandlerBox::OnProcessData(const uint8_t* body, size_t length) {
    preDefined  = GetValue<uint32_t>(body);
    handlerType = GetValue<uint32_t>(body + 4);
    for (size_t i = 0; i < sizeof(reserved) / sizeof(uint32_t); i++) {
        reserved[i] = GetValue<uint32_t>(body + 8 + 4 * i);
    }
    auto total = 8 + 4 * sizeof(reserved) / sizeof(uint32_t);
    std::string tmp(length - total, STRING_FILL);
    for (size_t i = 0; i < length - total; i++) {
        tmp[i] = static_cast<char>(*(body + total + i));
    }
    std::swap(name, tmp);
    return SUCCESS;
}

MusicStatus ExtendedLanguageBox::OnProcessData(const uint8_t* body, size_t length) {
    std::string tmp(length, STRING_FILL);
    for (size_t i = 0; i < length; i++) {
        tmp[i] = static_cast<char>(*(body + i));
    }
    std::swap(extendedLanguage, tmp);
    return SUCCESS;
}

MusicStatus SampleDescriptionBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    for (size_t i = 0; i < entryCount; i++) {
        // process SampleEntry
    }
    return SUCCESS;
}

MusicStatus DegradationPriorityBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length < 2) return ERROR_FILE_FORMAT;
    priorities   = std::make_unique<uint16_t[]>(length / 2);
    uint32_t pos = 0;
    while (length >= 2) {
        priorities[pos++]  = GetValue<int16_t>(body + pos * 2);
        length            -= 2;
    }
    return SUCCESS;
}

MusicStatus TimeToSampleBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (entryCount != (length - 4) / 8) return ERROR_FILE_FORMAT;
    sampleCounts = std::make_unique<uint32_t[]>(entryCount);
    sampleDelta  = std::make_unique<uint32_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        sampleCounts[i] = GetValue<uint32_t>(body + 4 + 8 * i);
        sampleDelta[i]  = GetValue<uint32_t>(body + 8 + 8 * i);
    }
    return SUCCESS;
}

MusicStatus CompositionOffsetBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (entryCount != (length - 4) / 8) return ERROR_FILE_FORMAT;
    sampleCount = std::make_unique<uint32_t[]>(entryCount);
    if (_header.version == 0) {
        sampleOffsetU = std::make_unique<uint32_t[]>(entryCount);
        for (size_t i = 0; i < entryCount; i++) {
            sampleCount[i]   = GetValue<uint32_t>(body + 4 + 8 * i);
            sampleOffsetU[i] = GetValue<uint32_t>(body + 8 + 8 * i);
        }
    } else { // version == 1
        sampleOffset = std::make_unique<int32_t[]>(entryCount);
        for (size_t i = 0; i < entryCount; i++) {
            sampleCount[i]  = GetValue<uint32_t>(body + 4 + 8 * i);
            sampleOffset[i] = GetValue<int32_t>(body + 8 + 8 * i);
        }
    }
    return SUCCESS;
}

MusicStatus CompositionToDecodeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (_header.version == 0) {
        MYASSERT(length == 20);
        compositionToDTSShift        = GetValue<int32_t>(body);
        leastDecodeToDisplayDelta    = GetValue<int32_t>(body + 4);
        greatestDecodeToDisplayDelta = GetValue<int32_t>(body + 8);
        compositionStartTime         = GetValue<int32_t>(body + 12);
        compositionEndTime           = GetValue<int32_t>(body + 16);
    } else { // version == 1
        MYASSERT(length == 40);
        compositionToDTSShift        = GetValue<int64_t>(body);
        leastDecodeToDisplayDelta    = GetValue<int64_t>(body + 8);
        greatestDecodeToDisplayDelta = GetValue<int64_t>(body + 16);
        compositionStartTime         = GetValue<int64_t>(body + 24);
        compositionEndTime           = GetValue<int64_t>(body + 32);
    }
    return SUCCESS;
}

MusicStatus SyncSampleBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (length != (entryCount * 4 + 4)) return ERROR_FILE_FORMAT;
    sampleNumber = std::make_unique<uint32_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        sampleNumber[i] = GetValue<uint32_t>(body + 4 + 4 * i);
    }
    return SUCCESS;
}

MusicStatus ShadowSyncSampleBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (length != (entryCount * 8 + 4)) return ERROR_FILE_FORMAT;
    shadowedSampleNumber = std::make_unique<uint32_t[]>(entryCount);
    syncSampleNumber     = std::make_unique<uint32_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        shadowedSampleNumber[i] = GetValue<uint32_t>(body + 4 + 8 * i);
        syncSampleNumber[i]     = GetValue<uint32_t>(body + 8 + 8 * i);
    }
    return SUCCESS;
}

MusicStatus SampleDependencyTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    bool flag1 = ParentCount(BoxType::STSZ) == 0;
    bool flag2 = ParentCount(BoxType::STZ2) == 0;
    if (flag1 && flag2) return ERROR_FILE_FORMAT;
    auto boxList = flag1 ? GetBoxList(BoxType::STZ2) : GetBoxList(BoxType::STSZ);
    MYASSERT(!boxList.empty())
    uint32_t entryCount = 0;
    if (!flag1) {
        auto stsz  = dynamic_pointer_cast<SampleSizeBox>(boxList.front());
        entryCount = stsz->sampleCount;
    } else {
        auto stz2  = dynamic_pointer_cast<CompactSampleSizeBox>(boxList.front());
        entryCount = stz2->sampleCount;
    }
    if (entryCount != length) return ERROR_FILE_FORMAT;
    infos = std::make_unique<SampleDependencyTypeInfo[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        auto tmp                     = GetValue<uint8_t>(body + i);
        infos[i].isLeading           = (tmp & 0b11000000) >> 6;
        infos[i].sampleDependsOn     = (tmp & 0b00110000) >> 4;
        infos[i].sampleIsDependedOn  = (tmp & 0b00001100) >> 2;
        infos[i].sampleHasRedundancy = tmp & 0b00000011;
    }
    return SUCCESS;
}

MusicStatus SampleSizeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length < 8) return ERROR_FILE_FORMAT;
    sampleSize  = GetValue<uint32_t>(body);
    sampleCount = GetValue<uint32_t>(body + 4);
    if (sampleSize != 0) return SUCCESS;
    entrySize = std::make_unique<uint32_t[]>(sampleCount);
    for (size_t i = 0; i < sampleCount; i++) {
        entrySize[i] = GetValue<uint32_t>(body + 8 + 4 * i);
    }
    return SUCCESS;
}

MusicStatus CompactSampleSizeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length < 8) return ERROR_FILE_FORMAT;
    fieldSize   = GetValue<uint32_t>(body) & 0x000000FF;
    sampleCount = GetValue<uint32_t>(body + 4);
    if (length == 8) return SUCCESS;
    entrySize = std::make_unique<uint8_t[]>(length - 8);
    memcpy_s(entrySize.get(), length - 8, body + 8, length - 8);
    return SUCCESS;
}

MusicStatus EditListBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue<uint32_t>(body);
    if (_header.version == 1) {
        segmentDuration64 = std::make_unique<uint64_t[]>(entryCount);
        mediaTime64       = std::make_unique<int64_t[]>(entryCount);
    } else { // version == 0
        segmentDuration32 = std::make_unique<uint32_t[]>(entryCount);
        mediaTime32       = std::make_unique<int32_t[]>(entryCount);
    }
    mediaRateInteger  = std::make_unique<int16_t[]>(entryCount);
    mediaRateFraction = std::make_unique<int16_t[]>(entryCount);
    auto tmpBody      = body + 4;
    for (size_t i = 0; i < entryCount; i++) {
        if (_header.version == 1) {
            segmentDuration64[i] = GetValue<uint64_t>(tmpBody);
            mediaTime64[i]       = GetValue<int64_t>(tmpBody + 8);
            tmpBody              = tmpBody + 16;
        } else { // version == 0
            segmentDuration32[i] = GetValue<uint32_t>(tmpBody);
            mediaTime32[i]       = GetValue<int32_t>(tmpBody + 4);
            tmpBody              = tmpBody + 8;
        }
        mediaRateInteger[i]  = GetValue<int16_t>(tmpBody);
        mediaRateFraction[i] = GetValue<int16_t>(tmpBody + 2);
        tmpBody              = tmpBody + 4;
    }
    return SUCCESS;
}

MusicStatus M4a::analysis() {
    try {
        std::fstream file(getFilePath(), ios::in | ios::binary);
        file.seekg(0, std::ios::end);
        size_t end = file.tellg();
        file.seekg(0, std::ios::beg);
        if (file.fail()) return ERROR_FILE_PATH;
        while (file.tellg() < end) {
#ifdef UTIL_DEBUG
            std::stringstream ss;
            ss << file.tellg();
            uint32_t pos = 0;
            ss >> pos;
#endif
            Box box;
            box.ProcessHeader(file);
#ifdef UTIL_DEBUG
            LOGI(" File SIZE:0x%x, \ttype:%s, pos:0x%x", box._header.size, box._header.TypeToString().c_str(), pos);
#endif
            auto iter = BoxMap.find(box._header.type);
            if (iter != BoxMap.end()) {
                auto subBox     = iter->second();
                subBox->_header = box._header;
                subBox->ProcessData(file);
                auto mapIter = _boxes.find(subBox->_header.type);
                if (mapIter == _boxes.end()) {
                    _boxes[subBox->_header.type] = {subBox};
                } else {
                    _boxes[subBox->_header.type].emplace_back(subBox);
                }
            } else {
                file.seekg(box._header.BodySize(), std::ios::cur);
            }
        }
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}
} // namespace myUtil
