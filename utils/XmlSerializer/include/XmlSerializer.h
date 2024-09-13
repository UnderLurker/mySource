//
// Created by 常笑男 on 2024/9/13.
//

#ifndef _XML_SERIALIZER_H
#define _XML_SERIALIZER_H

#include <list>
#include <memory>

namespace myUtil {
class XmlElement;
class XmlDocument {
public:
    XmlDocument()                              = delete;
    virtual ~XmlDocument()                     = default;
    XmlDocument& operator=(const XmlDocument&) = delete;

private:
    char* cache_ {nullptr};
    std::shared_ptr<XmlElement> root_ {nullptr};
};

struct XmlString {
    uintptr_t address {0};
    uint32_t length {0};
    std::string operator()() const {
        auto start = reinterpret_cast<char*>(address);
        return {start, start + length};
    }
};

struct XmlAttributes {
    std::shared_ptr<XmlString> name;
    std::shared_ptr<XmlString> value;
};

class XmlElement {
public:
    XmlElement()                             = delete;
    virtual ~XmlElement()                    = default;
    XmlElement& operator=(const XmlElement&) = delete;

private:
    std::list<XmlElement> children_;
    std::list<XmlAttributes> attribute_;
    std::shared_ptr<XmlString> name_ {nullptr};
};
} // namespace myUtil

#endif // _XML_SERIALIZER_H
