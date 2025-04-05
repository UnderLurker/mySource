//
// Created by CXN on 2025/2/6.
//
#include "m4a.h"

namespace myUtil {

MusicStatus M4a::analysis() {
    try {
        std::fstream file(getFilePath(), ios::in | ios::binary);
        file.seekg(0, std::ios::end);
        size_t end = file.tellg();
        file.seekg(0, std::ios::beg);
        if (file.fail()) return ERROR_FILE_PATH;
        while (file.tellg() < end) {
#ifdef UTIL_DEBUG
            std::stringstream ss;
            ss << file.tellg();
            uint32_t pos = 0;
            ss >> pos;
#endif
            Box box;
            box.ProcessHeader(file);
#ifdef UTIL_DEBUG
            LOGI(" File SIZE:0x%x, \ttype:%s, pos:0x%x", box._header.size, box._header.TypeToString().c_str(), pos);
#endif
            auto iter = BoxMap.find(box._header.type);
            if (iter != BoxMap.end()) {
                auto subBox     = iter->second();
                subBox->_header = box._header;
                subBox->ProcessData(file);
                auto mapIter = _boxes.find(subBox->_header.type);
                if (mapIter == _boxes.end()) {
                    _boxes[subBox->_header.type] = {subBox};
                } else {
                    _boxes[subBox->_header.type].emplace_back(subBox);
                }
            } else {
                file.seekg(box._header.BodySize(), std::ios::cur);
            }
        }
    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}
} // namespace myUtil
