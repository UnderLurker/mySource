//
// Created by 常笑男 on 2024/11/24.
//

#ifndef _XML_ELEMENT_H
#define _XML_ELEMENT_H

#include "xml_named_node_map.h"

namespace myUtil {

/**
 * @description Represents an element
 * @children Element, Text, Comment, ProcessingInstruction, CDATASection,
 * EntityReference
 */
class XmlElement : public XmlNodeObject {
public:
    XmlElement()                             = delete;
    virtual ~XmlElement()                    = default;
    XmlElement& operator=(const XmlElement&) = delete;

    /**
     * @details A NamedNodeMap containing the attributes of this node
     * @return
     */
    NamedNodeMap& Attribute() { return _attribute; }

private:
    NamedNodeMap _attribute;
};
} // namespace myUtil
#endif // _XML_ELEMENT_H
