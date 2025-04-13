//
// Created by 常笑男 on 2025/4/12.
//
#include "m4a/m4a_user_data.h"

#include <cstring>

namespace myUtil {

M4AStatus CopyrightBox::OnProcessData(const uint8_t* body, size_t length) {
    language = GetValue<uint16_t>(body);
    if (length <= 2) return ERROR_FILE_FORMAT;
    notice.resize(length - 2);
    memcpy_s(const_cast<char*>(notice.c_str()), notice.size(), body + 2, length - 2);
    return SUCCESS;
}

M4AStatus TrackSelectionBox::OnProcessData(const uint8_t* body, size_t length) {
    switchGroup = GetValue<int32_t>(body);
    if (length <= 4) return ERROR_FILE_FORMAT;
    attributeList = std::make_unique<uint32_t[]>(length - 4);
    for (uint32_t i = 0; i < length - 4; i++) {
        attributeList[i] = GetValue<uint32_t>(body + 4 + 4 * i);
    }
    return SUCCESS;
}

M4AStatus KindBox::OnProcessData(const uint8_t* body, size_t length) {
    uint32_t endPos = 0;
    for (size_t i = 0; i < length; i++) {
        if (*(body + i) == '\0') {
            endPos = i;
            break;
        }
    }
    schemeURI = std::string((char*)body);
    value     = std::string((char*)(body + endPos + 1));
    return SUCCESS;
}

M4AStatus XMLBox::OnProcessData(const uint8_t* body, size_t length) {
    xml = std::string((char*)body, length);
    return SUCCESS;
}

M4AStatus BinaryXMLBox::OnProcessData(const uint8_t* body, size_t length) {
    if (length == 0) return SUCCESS;
    len  = length;
    data = std::make_unique<uint8_t[]>(len);
    memcpy_s(data.get(), len, body, length);
    return SUCCESS;
}

M4AStatus ItemLocationBox::OnProcessData(const uint8_t* body, size_t length) {
    offsetSize     = *body >> 4;
    lengthSize     = *body & 0x0F;
    baseOffsetSize = *(body + 1) >> 4;
    if (_header.version == 1 || _header.version == 2) indexSize = *(body + 1) & 0x0F;
    uint32_t pos = 2;
    if (_header.version < 2) {
        itemCount = GetValue<uint16_t>(body + 2);
        pos       = 4;
    } else if (_header.version == 2) {
        itemCount = GetValue<uint32_t>(body + 2);
        pos       = 6;
    }
    if (itemCount == 0 || offsetSize == 0 || lengthSize == 0 || baseOffsetSize == 0) return SUCCESS;
    infos = std::make_unique<ItemLocationInfo[]>(itemCount);
    for (uint32_t i = 0; i < itemCount; i++) {
        if (_header.version < 2) {
            infos[i].itemId  = GetValue<uint16_t>(body + pos);
            pos             += 2;
        } else if (_header.version == 2) {
            infos[i].itemId  = GetValue<uint32_t>(body + pos);
            pos             += 4;
        }
        if (_header.version == 1 || _header.version == 2) {
            infos[i].constructionMethod  = GetValue<uint16_t>(body + pos);
            pos                         += 2;
        }
        infos[i].dataReferenceIndex = GetValue<uint16_t>(body + pos);
        infos[i].baseOffset         = std::make_unique<uint8_t[]>(baseOffsetSize);
        if ((_header.version == 1 || _header.version == 2) && indexSize > 0) {
            infos[i].extentIndex = std::make_unique<uint8_t[]>(indexSize);
        }
        infos[i].extentOffset = std::make_unique<uint8_t[]>(offsetSize);
        infos[i].extentLength = std::make_unique<uint8_t[]>(lengthSize);
        memcpy_s(infos[i].baseOffset.get(), baseOffsetSize, body + pos + 2, baseOffsetSize);
        infos[i].extentCount  = GetValue<uint16_t>(body + pos + 2 + baseOffsetSize);
        pos                  += 2 + baseOffsetSize;
        for (uint32_t j = 0; j < infos[i].extentCount; j++) {
            if (infos[i].extentIndex) {
                memcpy_s(infos[i].extentIndex.get(), indexSize, body + pos, indexSize);
                pos += indexSize;
            }
            memcpy_s(infos[i].extentOffset.get(), offsetSize, body + pos, offsetSize);
            memcpy_s(infos[i].extentLength.get(), lengthSize, body + pos + offsetSize, lengthSize);
            pos += offsetSize + lengthSize;
        }
    }

    return SUCCESS;
}

M4AStatus PrimaryItemBox::OnProcessData(const uint8_t* body, size_t length) {
    if (_header.version == 0) itemId = GetValue<uint16_t>(body);
    else itemId = GetValue<uint32_t>(body);
    return SUCCESS;
}

bool ItemProtectionBox::ProcessFullBox(std::fstream& file) {
    FullBox::ProcessFullBox(file);
    protectionCount = (file.get() << 8) | file.get();
    return true;
}

bool ItemInfoBox::ProcessFullBox(std::fstream& file) {
    FullBox::ProcessFullBox(file);
    if (_header.version == 0) entryCount = (file.get() << 8) | file.get();
    else entryCount = (file.get() << 24) | (file.get() << 16) | (file.get() << 8) | file.get();
    return true;
}

M4AStatus ItemInfoEntry::OnProcessData(const uint8_t* body, size_t length) {
    if (_header.version == 0 || _header.version == 1) {
        // Todo
    }
    return SUCCESS;
}
} // namespace myUtil
