//
// Created by Administrator on 2024/2/29.
//
#include "shader.h"

NAME_SPACE_START(myUtil)

Shader::Shader(const char* filePath) { _status = loadSource(_source, filePath); }

bool Shader::loadSource(char*& source, const std::string& filePath) {
    std::fstream file(filePath, std::ios::in | std::ios::binary);
    if (file.fail()) return false;
    file.seekg(0, std::fstream::end);
    size_t endPos = file.tellg();
    file.seekg(0, std::fstream::beg);
    source = new char[endPos + 1];
    file.read(source, static_cast<long long>(endPos));
    source[endPos] = '\0';
    return true;
}

NAME_SPACE_END()
