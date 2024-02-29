//
// Created by Administrator on 2024/2/29.
//

#ifndef MAIN_SHADER_H
#define MAIN_SHADER_H

#include <fstream>

#include "Util.h"

NAME_SPACE_START(myUtil)

class Shader {
public:
    Shader() = default;
    explicit Shader(const char* filePath);

    static bool loadSource(char*& source, const std::string& filePath);

public:
    bool _status {false};

private:
    char* _source {nullptr};
};

NAME_SPACE_END();

#endif // MAIN_SHADER_H
