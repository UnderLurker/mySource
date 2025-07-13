//
// Created by XW on 2024/3/16.
//

#ifndef _MUSIC_BASE_H
#define _MUSIC_BASE_H

#include <cassert>
#include <cstdint>
#include <cstring>

#include "music_const.h"
#include "Util.h"

namespace myUtil {
class MusicBase {
public:
    MusicBase() = default;
    explicit MusicBase(const char* filePath) {
        CHECK_NULL_VOID(filePath)
        MYASSERT(_filePath == nullptr);
        uint32_t len = strlen(filePath) + 1;
        _filePath    = new char[len];
        memcpy(_filePath, filePath, len - 1);
        _filePath[len - 1] = '\0';
    }
    virtual ~MusicBase() {
        delete[] _filePath;
        _filePath = nullptr;
    }
    virtual MusicStatus analysis() = 0;

    const char* getFilePath() const { return _filePath; }
    void setFilePath(const char* filePath) {
        CHECK_NULL_VOID(filePath)
        delete[] _filePath;
        _filePath    = nullptr;
        uint32_t len = strlen(filePath) + 1;
        _filePath    = new char[len];
        memcpy(_filePath, filePath, len - 1);
        _filePath[len - 1] = '\0';
    }

public:
    MusicStatus _status {ERROR_UNKNOWN};

private:
    char* _filePath {nullptr};
};
} // namespace myUtil

#endif // _MUSIC_BASE_H
