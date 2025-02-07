//
// Created by CXN on 2025/2/6.
//
#include "m4a.h"

namespace myUtil {
namespace {
uint64_t GetValue(const uint8_t* data, size_t begin, size_t end) {
    uint64_t result = 0;
    for (uint32_t i = begin; i < end; i++) {
        result = (result << 8) | data[i];
    }
    return result;
};

std::map<BoxType, std::function<std::shared_ptr<Box>()>> BoxMap = {
    {myUtil::BoxType::FTYP, []() { return std::make_shared<FtypBox>(); }},
    {myUtil::BoxType::MOOV, []() { return std::make_shared<MoovBox>(); }},
    {myUtil::BoxType::MVHD, []() { return std::make_shared<MvhdBox>(); }}
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

MusicStatus FtypBox::GetData(std::fstream& file) {
    auto remainLen = _header.BodySize();
    if (remainLen < 8) return ERROR_FILE_FORMAT;
    auto body = new uint8_t[remainLen];
    file.read((char*)body, remainLen);
    majorBrand    = GetValue(body, 0, 4);
    minorVersion  = GetValue(body, 4, 8);
    remainLen    -= 8;
    auto start    = 8;
    while (remainLen > 0) {
        compatibleBrands.push_back(GetValue(body, start, start + sizeof(uint32_t)));
        remainLen -= 4;
        start     += 4;
    }
    delete[] body;
    return SUCCESS;
}

MusicStatus MvhdBox::GetData(std::fstream& file) {
    auto remainLen = _header.BodySize();
    if (remainLen < 8) return ERROR_FILE_FORMAT;
    auto body = new uint8_t[remainLen];
    file.read((char*)body, remainLen);
    version          = GetValue(body, 0, 1);
    flags            = GetValue(body, 1, 4);
    createTime       = GetValue(body, 4, 8);
    modificationTime = GetValue(body, 8, 12);
    timeScale        = GetValue(body, 12, 16);
    duration         = GetValue(body, 16, 20);
    rate             = GetValue(body, 20, 24);
    volume           = GetValue(body, 24, 26);
    nextTrackId      = GetValue(body, 96, 100);
    memcpy(reserved, body + 26, 10);
    memcpy(matrix, body + 36, 36);
    memcpy(preDefined, body + 72, 24);
    delete[] body;
    return SUCCESS;
}

MusicStatus MoovBox::GetData(std::fstream& file) {
    int64_t remainLen = _header.BodySize();
    if (remainLen < 8) return ERROR_FILE_FORMAT;
    while (remainLen > 0) {
#ifdef UTIL_DEBUG
        std::stringstream ss;
        ss << file.tellg();
        uint32_t pos = 0;
        ss >> pos;
#endif
        Box box;
        box.GetData(file);
#ifdef UTIL_DEBUG
        LOGI(" SIZE:0x%x, \ttype:%s, pos:0x%x", box._header.size, box._header.TypeToString().c_str(), pos);
#endif
        auto bodySize = box._header.BodySize();
        auto iter     = BoxMap.find(box._header.type);
        if (iter == BoxMap.end()) {
            file.seekg(bodySize, std::ios::cur);
        } else {
            auto subBox     = iter->second();
            subBox->_header = box._header;
            subBox->GetData(file);
            boxes.emplace_back(subBox->_header.type, subBox);
        }
        remainLen -= bodySize;
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
            box.GetData(file);
#ifdef UTIL_DEBUG
            LOGI(" SIZE:0x%x, \ttype:%s, pos:0x%x", box._header.size, box._header.TypeToString().c_str(), pos);
#endif
            auto iter = BoxMap.find(box._header.type);
            if (iter != BoxMap.end()) {
                auto subBox     = iter->second();
                subBox->_header = box._header;
                subBox->GetData(file);
                _boxes.emplace_back(subBox->_header.type, subBox);
            } else {
                file.seekg(box._header.BodySize(), std::ios::cur);
            }
        }
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}
} // namespace myUtil
