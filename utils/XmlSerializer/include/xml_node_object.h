//
// Created by 常笑男 on 2024/11/24.
//

#ifndef _XML_NODE_OBJECT_H
#define _XML_NODE_OBJECT_H

#include <functional>
#include <list>

#include "xml_type.h"

namespace myUtil {
class XmlDocument;

struct UserData {
    void* userData;
    std::function<std::string(void*)> handler;
};

class XmlNodeObject {
public:
    XmlNodeObject() = default;
    explicit XmlNodeObject(const std::shared_ptr<XmlDocument>& doc, const std::shared_ptr<XmlNodeObject>& parent)
        : _doc(doc), _parent(parent) {}
    /**
     * @description Returns a NodeList of child nodes for a node
     * @return
     */
    std::list<std::shared_ptr<XmlNodeObject>>& ChildNodes() { return _children; }
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
     * @description Returns the type of a node
     * @return
     */
    [[nodiscard]] NodeType Type() const { return _nodeType; }
    /**
     * @description Returns the root element (document object) for a node
     * @return
     */
    std::shared_ptr<XmlDocument> OwnerDocument() { return _doc.lock(); }
    /**
     * @description Returns the parent node of a node
     * @return
     */
    std::shared_ptr<XmlNodeObject> ParentNode() { return _parent.lock(); }
    /**
     * @description Returns the namespace prefix of a node
     * @return
     */
    [[nodiscard]] std::string GetPrefix() const { return _prefix(); }
    /**
     * @description Returns the node immediately before a node
     * @return
     */
    std::shared_ptr<XmlNodeObject> GetPreviousSibling() { return _previousSibling.lock(); }
    /**
     * @description Returns the node immediately following a node
     * @return
     */
    std::shared_ptr<XmlNodeObject> GetNextSibling() { return _nextSibling.lock(); }
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
    /**
     * @description Returns the absolute base URI of a node
     * @return
     */
    std::string BaseURI() const;
    /**
     * @description Appends a new child node to the end of the list of children of a node
     * @param XmlNodeObject
     */
    void AppendChild(const std::shared_ptr<XmlNodeObject>& child);
    /**
     * @description Removes a specified child node from the current node
     * @param child
     */
    void RemoveChild(const std::shared_ptr<XmlNodeObject>& child) { _children.remove(child); }
//    /**
//     * @description Replaces a specified child node from the current node
//     * @param child
//     */
//    void ReplaceChild(const std::shared_ptr<XmlNodeObject&> value,
//                      const std::shared_ptr<XmlNodeObject&> newValue) {std::replace(
//        _children.begin(),
//        _children.end(),
//        [value](const std::shared_ptr<XmlNodeObject&> ele) { return ele == value; },
//        newValue)};
    /**
     * @description Clones a node
     * @return
     */
    std::shared_ptr<XmlNodeObject> Clone() const;
    /**
     * @description Compares the placement of two nodes in the DOM hierarchy (document)
     * @param XmlNodeObject
     * @return relative position
     */
    int32_t CompareDocumentPosition(const std::shared_ptr<XmlNodeObject>& obj);
    /**
     * @description Returns the object associated to a key on a this node.
     * The object must first have been set to this node by calling setUserData with the same key
     * @param key
     * @return
     */
    UserData GetUserData(const std::string& key) { return _userDataMap[key]; }
    /**
     * @description Associates an object to a key on a node
     * @param key
     * @param data
     */
    void SetUserData(const std::string& key, UserData data) { _userDataMap.emplace(key, data); }
    /**
     * @description Returns true if the specified node has any attributes, otherwise false
     * @return
     */
    virtual bool HasAttributes() const { return false; }
    /**
     * @description Returns true if the specified node has any child nodes, otherwise false
     * @return
     */
    virtual bool HasChildNodes() const { return !_children.empty(); }
    /**
     * @description Inserts a new child node before an existing child node
     * @param insert
     * @param position
     */
    void InsertBefore(const std::shared_ptr<XmlNodeObject>& insert, size_t position);
    /**
     * @description Returns whether the specified namespaceURI is the default
     * @return
     */
    bool IsDefaultNamespace() const;

    bool operator==(const XmlNodeObject& obj) const;


private:
    NodeType _nodeType { NodeType::NONE };
    std::list<std::shared_ptr<XmlNodeObject>> _children;
    XmlString _prefix;
    XmlString _nodeName;
    XmlString _nodeValue;
    XmlString _textContent;
    std::weak_ptr<XmlDocument> _doc;
    std::weak_ptr<XmlNodeObject> _parent;
    std::weak_ptr<XmlNodeObject> _previousSibling;
    std::weak_ptr<XmlNodeObject> _nextSibling;
    std::unordered_map<std::string, UserData> _userDataMap;
};

} // namespace myUtil

#endif // _XML_NODE_OBJECT_H
