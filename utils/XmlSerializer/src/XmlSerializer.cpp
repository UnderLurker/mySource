//
// Created by 常笑男 on 2024/9/13.
//
#include "XmlSerializer.h"

namespace myUtil {

XmlString& XmlElement::InnerText() const { return *(innerText_); }

XmlString& XmlDocument::InnerText() const {
    std::shared_ptr<XmlElement> shared(root_);
    return shared->InnerText();
}

} // namespace myUtil
