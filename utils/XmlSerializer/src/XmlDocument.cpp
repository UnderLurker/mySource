//
// Created by 常笑男 on 2024/11/24.
//
#include "xml_document.h"
namespace myUtil {

XmlString& XmlDocument::InnerText() const {
    std::shared_ptr<XmlElement> shared(root_);
    return shared->InnerText();
}

} // namespace myUtil