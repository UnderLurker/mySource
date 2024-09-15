//
// Created by 常笑男 on 2024/9/15.
//

#ifndef _XMLTYPE_H
#define _XMLTYPE_H

#include <cstdint>
#include <memory>
#include <string>

namespace myUtil {
template<typename T>
struct XmlType;

class XmlTypeBase {
public:
    XmlTypeBase()                              = delete;
    XmlTypeBase(const XmlTypeBase&)            = delete;
    XmlTypeBase& operator=(const XmlTypeBase&) = delete;
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
    explicit XmlType(uintptr_t address, uint32_t length)
        : XmlTypeBase(address, length) {}
    std::string operator()() const {
        auto start = reinterpret_cast<char*>(address_);
        return {start, start + length_};
    }
};

using XmlString = XmlType<std::string>;
using XmlInt32  = XmlType<int32_t>;
using XmlUInt32 = XmlType<uint32_t>;
using XmlInt64  = XmlType<int64_t>;
using XmlUInt64 = XmlType<uint64_t>;
using XmlBool   = XmlType<bool>;

struct XmlAttributes {
    std::shared_ptr<XmlString> name;
    std::shared_ptr<XmlString> value;
};

} // namespace myUtil

#endif // _XMLTYPE_H
