//
// Created by 常笑男 on 2024/11/24.
//

#ifndef _XML_NODE_OBJECT_H
#define _XML_NODE_OBJECT_H

#include <vector>

#include "xml_type.h"

namespace myUtil {
class XmlDocument;

class XmlNodeObject {
public:
    XmlNodeObject() = default;
    explicit XmlNodeObject(const std::shared_ptr<XmlDocument>& doc, const std::shared_ptr<XmlNodeObject>& parent)
        : _doc(doc), _parent(parent) {}
    std::vector<std::shared_ptr<XmlNodeObject>>& ChildNodes() { return _children; }
    std::shared_ptr<XmlNodeObject> FirstChild() { return _children.front(); }
    std::shared_ptr<XmlNodeObject> LastChild() { return _children.back(); }
    [[nodiscard]] std::string NodeName() const { return _nodeName(); }
    [[nodiscard]] std::string NodeValue() const { return _nodeValue(); }
    std::shared_ptr<XmlDocument> OwnerDocument() { return std::shared_ptr<XmlDocument>(_doc); }
    std::shared_ptr<XmlNodeObject> ParentNode() { return std::shared_ptr<XmlNodeObject>(_parent); }
    [[nodiscard]] std::string GetPrefix() const { return _prefix(); }
    std::shared_ptr<XmlNodeObject> GetPreviousSibling() { return std::shared_ptr<XmlNodeObject>(_previousSibling); }
    std::shared_ptr<XmlNodeObject> GetNextSibling() { return std::shared_ptr<XmlNodeObject>(_nextSibling); }
    [[nodiscard]] std::string GetTextContent() const { return _textContent(); }

    void SetPrefix(const std::string& name);
    void SetDocument(const std::shared_ptr<XmlDocument>& doc) { _doc = doc; }
    void SetParent(const std::shared_ptr<XmlNodeObject>& parent) { _parent = parent; }
    void SetPreviousSibling(const std::shared_ptr<XmlNodeObject>& pre) { _previousSibling = pre; }
    void SetNextSibling(const std::shared_ptr<XmlNodeObject>& next) { _nextSibling = next; }
    void SetTextContent(const std::string& textContent);

    void AppendChild(const std::shared_ptr<XmlNodeObject>& child);
    std::shared_ptr<XmlNodeObject> Clone() const;


private:
    NodeType _nodeType { NodeType::NONE };
    std::vector<std::shared_ptr<XmlNodeObject>> _children;
    XmlString _prefix;
    XmlString _nodeName;
    XmlString _nodeValue;
    XmlString _textContent;
    std::weak_ptr<XmlDocument> _doc;
    std::weak_ptr<XmlNodeObject> _parent;
    std::weak_ptr<XmlNodeObject> _previousSibling;
    std::weak_ptr<XmlNodeObject> _nextSibling;
};

} // namespace myUtil

#endif // _XML_NODE_OBJECT_H
