//
// Created by 常笑男 on 2024/9/15.
//

#ifndef _XML_TYPE_H
#define _XML_TYPE_H

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

namespace myUtil {
template<typename T>
struct XmlType;

class XmlTypeBase {
public:
    XmlTypeBase()                              = default;
    XmlTypeBase(const XmlTypeBase&)            = default;
    XmlTypeBase& operator=(const XmlTypeBase&) = default;
    explicit XmlTypeBase(uintptr_t address, uint32_t length)
        : address_(address), length_(length) {}
    XmlTypeBase(XmlTypeBase&& obj) noexcept {
        address_     = obj.address_;
        length_      = obj.length_;
        obj.address_ = obj.length_ = 0;
    }
    virtual ~XmlTypeBase() = default;

    template<typename K>
    XmlType<K> MoveType() {
        auto tempAdd = address_;
        auto tempLen = length_;
        address_ = length_ = 0;
        return {tempAdd, tempLen};
    }

protected:
    uintptr_t address_ {0};
    uint32_t length_ {0};
};

template<typename T>
struct XmlType : public XmlTypeBase {
    explicit XmlType(uintptr_t address, uint32_t length)
        : XmlTypeBase(address, length) {}
    const T& operator()() const {
        auto start = reinterpret_cast<T*>(address_);
        return *start;
    }
};

template<>
struct XmlType<std::string> : public XmlTypeBase {
    XmlType() = default;
    explicit XmlType(const char* address, uint32_t length)
        : XmlTypeBase(reinterpret_cast<uintptr_t>(address), length) {}
    explicit XmlType(uintptr_t address, uint32_t length)
        : XmlTypeBase(address, length) {}
    std::string operator()() const {
        auto start = reinterpret_cast<char*>(address_);
        return {start, start + length_};
    }
    bool operator==(const XmlType& obj) const {
        auto start1 = reinterpret_cast<char*>(address_);
        auto start2 = reinterpret_cast<char*>(obj.address_);
        bool result = strncmp(start1, start2, std::min(length_, obj.length_)) == 0;
        if (result && length_ == obj.length_) { return true; }
        return false;
    }
};

using XmlString = XmlType<std::string>;
using XmlInt32  = XmlType<int32_t>;
using XmlUInt32 = XmlType<uint32_t>;
using XmlInt64  = XmlType<int64_t>;
using XmlUInt64 = XmlType<uint64_t>;
using XmlFloat  = XmlType<float>;
using XmlDouble = XmlType<double>;
using XmlBool   = XmlType<bool>;

struct XmlAttributes {
    std::shared_ptr<XmlString> name;
    std::shared_ptr<XmlString> value;
};

struct XmlHeader {
    XmlAttributes version;
    XmlAttributes encoding;
};

enum NodeType {
    NONE = 0,
    ELEMENT_NODE,
    ATTRIBUTE_NODE,
    TEXT_NODE,
    CDATA_SECTION_NODE,
    ENTITY_REFERENCE_NODE,
    ENTITY_NODE,
    PROCESSING_INSTRUCTION_NODE,
    COMMENT_NODE,
    DOCUMENT_NODE,
    DOCUMENT_TYPE_NODE,
    DOCUMENT_FRAGMENT_NODE,
    NOTATION_NODE
};

} // namespace myUtil

template<>
struct std::hash<myUtil::XmlString> {
    size_t operator()(const myUtil::XmlString& s) const noexcept { return hash<std::string>()(s()); }
};
#endif // _XML_TYPE_H
