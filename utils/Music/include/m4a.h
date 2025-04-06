//
// Created by CXN on 2025/2/6.
//

#ifndef _M4A_H
#define _M4A_H

#include <fstream>

#include "m4a/m4a_map.h"
#include "music_base.h"

namespace myUtil {

class M4a : public MusicBase {
public:
    explicit M4a(const char* filePath)
        : MusicBase(filePath) {
        _status = analysis();
    }
    MusicStatus analysis() override;

    ostringstream PrintTree(uint32_t tab, uint32_t count = 2);
private:
    Box::SubBoxMap _boxes;
};

} // namespace myUtil

#endif // _M4A_H
