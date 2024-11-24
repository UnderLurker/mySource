//
// Created by 常笑男 on 2024/11/24.
//

#ifndef _XML_NAMED_NODE_MAP_H
#define _XML_NAMED_NODE_MAP_H

#include <cstdint>
#include <unordered_map>

#include "xml_node_object.h"
#include "xml_type.h"

namespace myUtil {

/**
 * @description Only XmlElement
 */
class NamedNodeMap {
public:
    /**
     * @details Returns the number of nodes in the list
     * @return
     */
    size_t Length() const { return _nodeMap.size(); }

    /**
     * @details Returns the node with the specific name
     * @param name
     * @return
     */
    std::shared_ptr<XmlNodeObject> GetNamedItem(const std::string& name);
    /**
     * @details Returns the node with the specific name and namespace
     * @param name
     * @param spaceName namespaceName
     * @return
     */
    std::shared_ptr<XmlNodeObject> GetNamedItemNS(const std::string& name, const std::string& spaceName);
    /**
     * @details Returns the node at the specified index
     * @param index
     * @return
     */
    std::shared_ptr<XmlNodeObject> Item(size_t index);
    /**
     * @details Removes the node with the specific name
     * @param name
     */
    void RemoveNamedItem(const std::string& name);
    /**
     * @details Removes the node with the specific name and namespace
     * @param name
     * @param spaceName
     */
    void RemoveNamedItemNS(const std::string& name, const std::string& spaceName);
    /**
     * @details Sets the specified node (by name)
     * @param name
     */
    void SetNamedItem(const std::string& name, std::shared_ptr<XmlNodeObject>);
    /**
     * Sets the specified node (by name and namespace)
     * @param name
     * @param spaceName
     */
    void SetNamedItemNS(const std::string& name, const std::string& spaceName, std::shared_ptr<XmlNodeObject>);

private:
    std::unordered_map<XmlString, std::shared_ptr<XmlNodeObject>> _nodeMap;
};

} // namespace myUtil

#endif // _XML_NAMED_NODE_MAP_H
