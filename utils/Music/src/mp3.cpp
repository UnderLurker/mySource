#include "mp3.h"

#include <fstream>

#include "include/logger_wrapper.h"

using namespace std;
NAME_SPACE_START(myUtil)

MusicStatus MP3::analysis() {
    try {
        fstream file(getFilePath(), ios::in | ios::binary);
        file.seekg(0, std::ios::end);
        size_t end = file.tellg();
        file.seekg(0, std::ios::beg);
        if (file.fail()) return ERROR_FILE_PATH;
        _id3v2.readData(file);
        if (_id3v2._header.header != IDV3) return ERROR_FILE_FORMAT;
        LOGI("File format right!!!");
        while (true) {
            MusicFrame frame;
            if (!frame.readData(file)) break;
            LOGD("%d %d %d %d", frame._header._header.data1, frame._header._header.data2, frame._header._header.data3,
                 frame._header._header.data4);
            _frames.push_back(frame);
            if (file.tellg() >= end) break;
        }

        file.seekg(-ID3V1_LEN, std::ios::end);
        auto data = std::make_unique<uint8_t[]>(ID3V1_LEN);
        file.read((char*)data.get(), ID3V1_LEN);
        if (strncmp(reinterpret_cast<const char*>(data.get()), "TAG", 3) == 0) {
            _id3v1.setValue(data.get(), ID3V1_LEN);
        }

    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

NAME_SPACE_END()
