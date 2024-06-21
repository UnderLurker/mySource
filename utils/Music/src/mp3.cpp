#include "mp3.h"

#include <fstream>

using namespace std;
NAME_SPACE_START(myUtil)

MusicStatus MP3::analysis() {
    try {
        fstream file(getFilePath(), ios::in | ios::binary);
        if (file.fail()) return ERROR_FILE_PATH;
        _id3v2.readData(file);
        if (_id3v2._header.header != IDV3) return ERROR_FILE_FORMAT;
        while (1) {
            MusicFrame frame;
            frame.readData(file);
            _frames.push_back(frame);
        }
        file.seekg(-ID3V1_LEN, std::ios::end);
        std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(ID3V1_LEN);
        file.read((char*)data.get(), ID3V1_LEN);
        _id3v1.setValue(data.get(), ID3V1_LEN);

    } catch (...) { return ERROR_UNKNOWN; }
    return SUCCESS;
}

NAME_SPACE_END()
