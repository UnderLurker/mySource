//
// Created by 常笑男 on 2024/11/24.
//
#include "xml_named_node_map.h"

namespace myUtil {
std::shared_ptr<XmlNodeObject> NamedNodeMap::GetNamedItem(const std::string& name) {
    XmlString key(name.c_str(), name.length());
    return _nodeMap[std::move(key)];
}
}