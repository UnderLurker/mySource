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
    {myUtil::BoxType::MVHD, []() { return std::make_shared<MvhdBox>(); }},
    {myUtil::BoxType::MDAT, []() { return std::make_shared<MdatBox>(); }},
    {myUtil::BoxType::FREE, []() { return std::make_shared<FreeBox>(); }},
    {myUtil::BoxType::SKIP, []() { return std::make_shared<SkipBox>(); }},
    {myUtil::BoxType::PDIN, []() { return std::make_shared<PdinBox>(); }},
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

void Box::ProcessFullBox(std::fstream& file) {
    auto data = new int8_t[4];
    file.read((char*)data, 4);
    ProcessFullBox(reinterpret_cast<const uint8_t*>(data), 4);
    delete[] data;
}
void Box::ProcessFullBox(const uint8_t* body, size_t length) {
    _header.version = body[0];
    _header.flags   = GetValue(body, 1, 4);
}

MusicStatus LeafBox::ProcessData(std::fstream& file) {
    auto remainLen = _header.BodySize();
    if (remainLen < 8) return ERROR_FILE_FORMAT;
    auto body = new uint8_t[remainLen];
    file.read((char*)body, remainLen);
    auto ret = OnProcessData(body, remainLen);
    delete[] body;
    return ret;
}

MusicStatus FtypBox::OnProcessData(const uint8_t* body, size_t length) {
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

MusicStatus MvhdBox::OnProcessData(const uint8_t* body, size_t length) {
    ProcessFullBox(body, length);
    if ((_header.version == 1 && length != 112) || (_header.version == 0 && length != 100)) return ERROR_FILE_FORMAT;
    auto pos = 0;
    if (_header.version == 1) {
        createTime       = GetValue(body + FULL_BOX_LEN, 0, 8);
        modificationTime = GetValue(body + FULL_BOX_LEN + 8, 0, 8);
        timeScale        = GetValue(body + FULL_BOX_LEN + 16, 0, 4);
        duration         = GetValue(body + FULL_BOX_LEN + 20, 0, 8);
        pos              = FULL_BOX_LEN + 28;
    } else { // version == 0
        createTime       = GetValue(body + FULL_BOX_LEN, 0, 4);
        modificationTime = GetValue(body + FULL_BOX_LEN + 4, 0, 4);
        timeScale        = GetValue(body + FULL_BOX_LEN + 8, 0, 4);
        duration         = GetValue(body + FULL_BOX_LEN + 12, 0, 4);
        pos              = FULL_BOX_LEN + 16;
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

MusicStatus MdatBox::OnProcessData(const uint8_t* body, size_t length) {
    data = std::make_unique<uint8_t[]>(length);
    memcpy(data.get(), body, length);
    len = length;
    return SUCCESS;
}

MusicStatus PdinBox::OnProcessData(const uint8_t* body, size_t length) {
    ProcessFullBox(body, length);
    size_t size = sizeof(PdinInfo);
    if (length - FULL_BOX_LEN % size != 0) return ERROR_FILE_FORMAT;
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
