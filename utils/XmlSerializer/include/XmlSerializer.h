//
// Created by 常笑男 on 2024/9/13.
//

#ifndef _XML_SERIALIZER_H
#define _XML_SERIALIZER_H

#include <list>
#include <memory>
#include <string>

#include "XmlType.h"

namespace myUtil {
class XmlDocument;
class XmlElement {
public:
    XmlElement()                             = delete;
    virtual ~XmlElement()                    = default;
    XmlElement& operator=(const XmlElement&) = delete;

    [[nodiscard]] XmlString& InnerText() const;

private:
    std::shared_ptr<XmlDocument> doc_;
    std::list<XmlElement> children_;
    std::list<XmlAttributes> attribute_;
    std::shared_ptr<XmlString> tagName_ {nullptr};
    std::unique_ptr<XmlString> innerText_ {nullptr};
};

class XmlDocument {
public:
    XmlDocument()                              = delete;
    virtual ~XmlDocument()                     = default;
    XmlDocument& operator=(const XmlDocument&) = delete;

    [[nodiscard]] XmlString& InnerText() const;

private:
    char* cache_ {nullptr};
    std::weak_ptr<XmlElement> root_;
};

} // namespace myUtil

#endif // _XML_SERIALIZER_H
