//
// Created by 常笑男 on 2024/11/24.
//

#ifndef _XML_DOCUMENT_FRAGMENT_H
#define _XML_DOCUMENT_FRAGMENT_H

#include "xml_node_object.h"

namespace myUtil {

/**
 * @description Represents a "lightweight" Document object, which can hold a portion of a document
 * @children Element, ProcessingInstruction, Comment, Text, CDATASection, EntityReference
 */
class XmlDocumentFragment : public XmlNodeObject {
public:
private:
};

} // namespace myUtil

#endif // _XML_DOCUMENT_FRAGMENT_H
