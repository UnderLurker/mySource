//
// Created by 常笑男 on 2024/11/24.
//

#ifndef _XML_DOCUMENT_H
#define _XML_DOCUMENT_H

#include "xml_element.h"
#include "xml_type.h"

namespace myUtil {

/**
 * @description Represents the entire document (the root-node of the DOM tree)
 * @children Element (max. one), ProcessingInstruction, Comment, DocumentType
 *
 */
class XmlDocument {
public:
    XmlDocument()                              = delete;
    virtual ~XmlDocument()                     = default;
    XmlDocument& operator=(const XmlDocument&) = delete;

    [[nodiscard]] XmlString& InnerText() const;

private:
    char* cache_ {nullptr};
    std::shared_ptr<XmlElement> root_;
};
} // namespace myUtil
#endif // _XML_DOCUMENT_H
