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
    /**
     * @description Returns a NodeList of child nodes for a node
     * @return
     */
    std::vector<std::shared_ptr<XmlNodeObject>>& ChildNodes() { return _children; }
    /**
     * @description Returns the first child of a node
     * @return
     */
    std::shared_ptr<XmlNodeObject> FirstChild() { return _children.front(); }
    /**
     * @description Returns the last child of a node
     * @return
     */
    std::shared_ptr<XmlNodeObject> LastChild() { return _children.back(); }
    /**
     * @description Returns the name of a node, depending on its type
     * @return
     */
    [[nodiscard]] std::string NodeName() const { return _nodeName(); }
    /**
     * @description returns the value of a node, depending on its type
     * @return
     */
    [[nodiscard]] std::string NodeValue() const { return _nodeValue(); }
    /**
     * @description Returns the root element (document object) for a node
     * @return
     */
    std::shared_ptr<XmlDocument> OwnerDocument() { return std::shared_ptr<XmlDocument>(_doc); }
    /**
     * @description Returns the parent node of a node
     * @return
     */
    std::shared_ptr<XmlNodeObject> ParentNode() { return std::shared_ptr<XmlNodeObject>(_parent); }
    /**
     * @description Returns the namespace prefix of a node
     * @return
     */
    [[nodiscard]] std::string GetPrefix() const { return _prefix(); }
    /**
     * @description Returns the node immediately before a node
     * @return
     */
    std::shared_ptr<XmlNodeObject> GetPreviousSibling() { return std::shared_ptr<XmlNodeObject>(_previousSibling); }
    /**
     * @description Returns the node immediately following a node
     * @return
     */
    std::shared_ptr<XmlNodeObject> GetNextSibling() { return std::shared_ptr<XmlNodeObject>(_nextSibling); }
    /**
     * @description Returns the textual content of a node and its descendants
     * @return
     */
    [[nodiscard]] std::string GetTextContent() const { return _textContent(); }
    /**
     * @description Sets the namespace prefix of a node
     * @param name
     */
    void SetPrefix(const std::string& name);
    /**
     * @description Set the root element (document object) for a node
     * @param document
     */
    void SetDocument(const std::shared_ptr<XmlDocument>& doc) { _doc = doc; }
    /**
     * @description Set the parent node of a node
     * @param parent
     */
    void SetParent(const std::shared_ptr<XmlNodeObject>& parent) { _parent = parent; }
    /**
     * @description Set the node immediately before a node
     * @param preNode
     */
    void SetPreviousSibling(const std::shared_ptr<XmlNodeObject>& pre) { _previousSibling = pre; }
    /**
     * @description Set the node immediately following a node
     * @param nextNode
     */
    void SetNextSibling(const std::shared_ptr<XmlNodeObject>& next) { _nextSibling = next; }
    /**
     * @description Set the textual content of a node and its descendants
     * @param textContent
     */
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
