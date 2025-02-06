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
    {myUtil::FTYP, []() { return std::make_shared<FtypBox>(); }}
};
} // namespace

std::string BoxHeader::TypeToString() const {
    std::string res;
    auto tmp = _type;
    while (tmp) {
        res.append({(char)(tmp & 0xFF)});
        tmp >>= 8;
    }
    std::reverse(res.begin(), res.end());
    return res;
}

void BoxHeader::ReadData(const uint8_t* data, size_t length) {
    MYASSERT(length == BOX_HEADER_LEN)
    _size      = GetValue(data, 0, 4);
    _type      = GetValue(data, 4, 8);
    _largeSize = GetValue(data, 8, length);
}

MusicStatus FtypBox::GetData(std::fstream& file) {
    auto remainLen = _header.Size() - BOX_HEADER_LEN;
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
            Box box;
            box.GetData(file);
            auto iter = BoxMap.find(box._header.Type());
            if (iter != BoxMap.end()) {
                auto subBox     = iter->second();
                subBox->_header = box._header;
                subBox->GetData(file);
                _boxes.emplace_back(subBox->_header.Type(), subBox);
            } else {
                file.seekg(box._header.Size() - sizeof(BoxHeader), std::ios::cur);
            }
#ifdef UTIL_DEBUG
            std::stringstream ss;
            ss << file.tellg();
            uint32_t pos = 0;
            ss >> pos;
            LOGI("\tSIZE:0x%x, \ttype:%s, \tpos:0x%x", box._header.Size(), box._header.TypeToString().c_str(), pos);
#endif
        }
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}
} // namespace myUtil
