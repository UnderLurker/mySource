//
// Created by CXN on 2025/2/6.
//
#include "m4a.h"

namespace myUtil {
namespace {
constexpr static char STRING_FILL = ' ';
uint64_t GetValue(const uint8_t* data, size_t begin, size_t end) {
    uint64_t result = 0;
    for (uint32_t i = begin; i < end; i++) {
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
    {myUtil::BoxType::STDP, []() { return std::make_shared<DegradationPriorityBox>(); }      },
    {myUtil::BoxType::STTS, []() { return std::make_shared<TimeToSampleBox>(); }      },
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
    size = GetValue(data, 0, 4);
    type = static_cast<BoxType>(GetValue(data, 4, 8));
}

void BoxHeader::SetBigSize(const uint8_t* data, size_t length) {
    MYASSERT(length == BOX_HEADER_LEN)
    size = GetValue(data, 0, 8);
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
            userType = new uint8_t[USER_TYPE_LEN];
            file.read((char*)userType, 16);
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
            subBox->ProcessData(file);
            boxes.emplace_back(subBox->_header.type, subBox);
        }
        remainLen -= bodySize;
    }
    return SUCCESS;
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
    _header.flags   = GetValue(body, 1, 4);
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
    majorBrand    = GetValue(body, 0, 4);
    minorVersion  = GetValue(body, 4, 8);
    length       -= 8;
    auto start    = 8;
    while (length > 0) {
        compatibleBrands.push_back(GetValue(body, start, start + sizeof(uint32_t)));
        length -= sizeof(uint32_t);
        start  += sizeof(uint32_t);
    }
    return SUCCESS;
}

MusicStatus MovieHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    if ((_header.version == 1 && length != 108) || (_header.version == 0 && length != 96)) return ERROR_FILE_FORMAT;
    auto pos = 0;
    if (_header.version == 1) {
        createTime       = GetValue(body, 0, 8);
        modificationTime = GetValue(body + 8, 0, 8);
        timeScale        = GetValue(body + 16, 0, 4);
        duration         = GetValue(body + 20, 0, 8);
        pos              = 28;
    } else { // version == 0
        createTime       = GetValue(body, 0, 4);
        modificationTime = GetValue(body + 4, 0, 4);
        timeScale        = GetValue(body + 8, 0, 4);
        duration         = GetValue(body + 12, 0, 4);
        pos              = 16;
    }
    rate        = GetValue(body + pos, 0, 4);
    volume      = GetValue(body + pos + 4, 0, 2);
    nextTrackId = GetValue(body + pos + 76, 0, 4);
    memcpy(reserved, body + pos + 6, 10);
    memcpy(matrix, body + pos + 16, 36);
    for (size_t i = 0; i < sizeof(matrix) / sizeof(int32_t); i++) {
        auto tmp  = body + pos + 16 + 4 * i;
        matrix[i] = GetValue(tmp, 0, 4);
    }
    for (size_t i = 0; i < sizeof(preDefined) / sizeof(int32_t); i++) {
        auto tmp      = body + pos + 52 + 4 * i;
        preDefined[i] = GetValue(tmp, 0, 4);
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
        uint32_t rate         = GetValue(body, l, r);
        uint32_t initialDelay = GetValue(body, l + 4, r + 4);
        infos.push_back({rate, initialDelay});
        length -= size;
        l      += size;
        r      += size;
    }
    return SUCCESS;
}

MusicStatus TrackHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    const uint8_t* tmpBody = body;
    if (_header.version == 1) {
        creationTime     = GetValue(tmpBody, 0, 8);
        modificationTime = GetValue(tmpBody + 8, 0, 8);
        trackId          = GetValue(tmpBody + 16, 0, 4);
        reserved         = GetValue(tmpBody + 20, 0, 4);
        duration         = GetValue(tmpBody + 24, 0, 8);
        tmpBody          = tmpBody + 32;
    } else { // version == 0
        creationTime     = GetValue(tmpBody, 0, 4);
        modificationTime = GetValue(tmpBody + 4, 0, 4);
        trackId          = GetValue(tmpBody + 8, 0, 4);
        reserved         = GetValue(tmpBody + 12, 0, 4);
        duration         = GetValue(tmpBody + 16, 0, 4);
        tmpBody          = tmpBody + 20;
    }
    tmpBody        = tmpBody + 8;
    layer          = GetValue(tmpBody, 0, 2);
    alternateGroup = GetValue(tmpBody + 2, 0, 2);
    volume         = GetValue(tmpBody + 4, 0, 2);
    width          = GetValue(tmpBody + 44, 0, 4);
    height         = GetValue(tmpBody + 48, 0, 4);
    for (size_t i = 0; i < sizeof(matrix) / sizeof(int32_t); i++) {
        auto tmp  = tmpBody + 8 + 4 * i;
        matrix[i] = GetValue(tmp, 0, 4);
    }
    return SUCCESS;
}

MusicStatus TrackReferenceTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length % 4 != 0) return ERROR_FILE_FORMAT;
    len      = length / 4;
    trackIDs = std::make_unique<uint32_t[]>(len);
    for (size_t i = 0; i < length; i += 4) {
        trackIDs[i] = GetValue(body + i, 0, 4);
    }
    return SUCCESS;
}

MusicStatus TrackGroupTypeBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length != sizeof(trackGroupId)) return ERROR_FILE_FORMAT;
    trackGroupId = GetValue(body, 0, 4);
    return SUCCESS;
}

MusicStatus MediaHeaderBox::OnProcessData(const uint8_t* body, size_t length) {
    auto tmpBody = body;
    if (_header.version == 1) {
        creationTime      = GetValue(tmpBody, 0, 8);
        modificationTime  = GetValue(tmpBody + 8, 0, 8);
        timeScale         = GetValue(tmpBody + 16, 0, 4);
        duration          = GetValue(tmpBody + 20, 0, 8);
        tmpBody          += 28;
    } else { // version == 0;
        creationTime      = GetValue(tmpBody, 0, 4);
        modificationTime  = GetValue(tmpBody + 4, 0, 4);
        timeScale         = GetValue(tmpBody + 8, 0, 4);
        duration          = GetValue(tmpBody + 12, 0, 4);
        tmpBody          += 16;
    }
    pad        = (tmpBody[0] & 0x80) == 0x80;
    language   = GetValue(tmpBody, 0, 2) | 0x7F;
    preDefined = GetValue(tmpBody + 2, 0, 2);
    return SUCCESS;
}

MusicStatus HandlerBox::OnProcessData(const uint8_t* body, size_t length) {
    preDefined  = GetValue(body, 0, 4);
    handlerType = GetValue(body + 4, 0, 4);
    for (size_t i = 0; i < sizeof(reserved) / sizeof(uint32_t); i++) {
        reserved[i] = GetValue(body + 8 + 4 * i, 0, 4);
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
    entryCount = GetValue(body, 0, 4);
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
        priorities[pos++]  = GetValue(body + pos * 2, 0, 2);
        length            -= 2;
    }
    return SUCCESS;
}

MusicStatus TimeToSampleBox::OnProcessData(const uint8_t* body, size_t length) {
    entryCount = GetValue(body, 0, 4);
    if (entryCount != (length - 4) / 8) return ERROR_FILE_FORMAT;
    sampleCounts = std::make_unique<uint32_t[]>(entryCount);
    sampleDelta  = std::make_unique<uint32_t[]>(entryCount);
    for (size_t i = 0; i < entryCount; i++) {
        sampleCounts[i] = GetValue(body + 4 + 8 * i, 0, 4);
        sampleDelta[i]  = GetValue(body + 8 + 8 * i, 0, 4);
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
                _boxes.emplace_back(subBox->_header.type, subBox);
            } else {
                file.seekg(box._header.BodySize(), std::ios::cur);
            }
        }
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}
} // namespace myUtil
