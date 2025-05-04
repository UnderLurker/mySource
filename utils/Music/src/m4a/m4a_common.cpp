//
// Created by 常笑男 on 2025/4/4.
//
#include "m4a/m4a_common.h"

#include <cstring>

#include "m4a/m4a_map.h"

namespace myUtil {

std::string TypeToString(uint32_t type) {
    std::string res;
    auto tmp = (uint32_t)type;
    while (tmp) {
        res.append({(char)(tmp & 0xFF)});
        tmp >>= 8;
    }
    std::reverse(res.begin(), res.end());
    return res;
}

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
M4AStatus Box::ProcessHeader(std::fstream& file) {
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
            file.read((char*)_userType, USER_TYPE_LEN);
        }
        delete[] data;
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

M4AStatus Box::ProcessHeader(const uint8_t* body, size_t length) {
    try {
        _header.SetValue(body, BOX_HEADER_LEN);
        body    = body + BOX_HEADER_LEN;
        length -= BOX_HEADER_LEN;
        if (_header.size == 1 && length >= BOX_HEADER_LEN) {
            _header.SetBigSize(body, BOX_HEADER_LEN);
            body    = body + BOX_HEADER_LEN;
            length -= BOX_HEADER_LEN;
        }
        if (_header.type == myUtil::BoxType::UUID && length >= USER_TYPE_LEN) {
            _userType = new uint8_t[USER_TYPE_LEN];
            memcpy_s(_userType, USER_TYPE_LEN, body, USER_TYPE_LEN);
        }
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

M4AStatus Box::ProcessData(std::fstream& file) {
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
        remainLen -= box._header.size;
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

ostringstream Box::PrintTree(uint32_t tab, const std::string& fill) {
    ostringstream ss;
    for(const auto& item : _boxes) {
        auto tmp = tab;
        while(tmp--) {
            ss << fill;
        }
        ss << TypeToString(item.first) << endl;
        for(const auto& box : item.second) {
            if (box) {
                ss << box->PrintTree(tab + 1, fill).str();
            }
        }
    }
    return ss;
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

M4AStatus LeafBox::ProcessData(std::fstream& file) {
    auto remainLen = _header.BodySize();
    if (remainLen < 8) return ERROR_FILE_FORMAT;
    auto body = new uint8_t[remainLen];
    file.read((char*)body, remainLen);
    M4AStatus ret;
    if (ProcessFullBox(body, FULL_BOX_LEN)) {
        ret = OnProcessData(body + FULL_BOX_LEN, remainLen - FULL_BOX_LEN);
    } else {
        ret = OnProcessData(body, remainLen);
    }
    delete[] body;
    return ret;
}

M4AStatus FileTypeBox::OnProcessData(const uint8_t* body, size_t length) {
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
} // namespace myUtil
