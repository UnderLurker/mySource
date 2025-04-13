//
// Created by 常笑男 on 2025/4/12.
//

#ifndef _M4A_USER_DATA_H
#define _M4A_USER_DATA_H

#include "m4a/m4a_common.h"

namespace myUtil {
// UDTA
struct UserDataBox : public Box {};
// CPRT
struct CopyrightBox : public LeafFullBox {
    uint16_t language; // ISO-639-2/T language code
    std::string notice;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// TSEL
struct TrackSelectionBox : public LeafFullBox {
    int32_t switchGroup;
    std::unique_ptr<uint32_t[]> attributeList;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// KIND
struct KindBox : public LeafFullBox {
    std::string schemeURI;
    std::string value;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// META
struct MetaBox : public FullBox {};
// 'XML '
struct XMLBox : public LeafFullBox {
    std::string xml;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// BXML
struct BinaryXMLBox : public LeafFullBox {
    uint32_t len {0};
    std::unique_ptr<uint8_t[]> data;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// ILOC
struct ItemLocationBox : public LeafFullBox {
    struct ItemLocationInfo {
        uint32_t itemId;
        uint8_t constructionMethod; // 0(file), 1(idat), 2(item)
        uint16_t dataReferenceIndex;
        std::unique_ptr<uint8_t[]> baseOffset;
        uint16_t extentCount;
        std::unique_ptr<uint8_t[]> extentIndex;
        std::unique_ptr<uint8_t[]> extentOffset;
        std::unique_ptr<uint8_t[]> extentLength;
    };
    uint16_t offsetSize     : 4; // {0, 4, 8}
    uint16_t lengthSize     : 4; // {0, 4, 8}
    uint16_t baseOffsetSize : 4; // {0, 4, 8}
    uint16_t indexSize      : 4; // {0, 4, 8}
    uint32_t itemCount {0};
    std::unique_ptr<ItemLocationInfo[]> infos;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// PITM
struct PrimaryItemBox : public LeafFullBox {
    uint32_t itemId;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};
// IPRO
struct ItemProtectionBox : public FullBox {
    uint16_t protectionCount;
    bool ProcessFullBox(std::fstream& file) override;
};
// IINF
struct ItemInfoBox : public FullBox {
    uint32_t entryCount {0};
    bool ProcessFullBox(std::fstream& file) override;
};
// INFE
struct ItemInfoEntry : public LeafFullBox {
    struct FDItemInfoExtension { // extension_type == 'fdel'
        std::string contentLocation;
        std::string contentMD5;
        uint64_t contentLength;
        uint64_t transferLength;
        uint8_t entryCount;
        std::unique_ptr<uint32_t[]> groupId;
    };
    std::optional<uint32_t> itemId;
    std::optional<uint16_t> itemProtectionIndex;
    std::optional<std::string> itemName;
    std::optional<std::string> contentType;
    std::optional<std::string> contentEncoding;
    std::optional<uint32_t> extensionType;
    std::optional<FDItemInfoExtension> extensionInfo;
    std::optional<uint32_t> itemType;
    std::optional<std::string> itemUriType;
    M4AStatus OnProcessData(const uint8_t* body, size_t length) override;
};

} // namespace myUtil

#endif // _M4A_USER_DATA_H
