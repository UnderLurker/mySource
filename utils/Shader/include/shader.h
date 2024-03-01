//
// Created by Administrator on 2024/2/29.
//

#ifndef MAIN_SHADER_H
#define MAIN_SHADER_H

#include <fstream>

#include "Util.h"

NAME_SPACE_START(myUtil)
#define MSG_SIZE 512

enum ShaderType {
    FRAGMENT_SHADER = 0x8B30,
    VERTEX_SHADER = 0x8B31
};

class Shader {
public:
    Shader() = delete;
    explicit Shader(const char* filePath, ShaderType shaderType);
    virtual ~Shader() = default;

    bool loadSource(const std::string& filePath);
    void createShader(ShaderType shaderType);
    bool compile();
    void deleteShader();

public:
    bool _status {true};
    uint32_t _shaderId {0};

private:
    ShaderType _type {VERTEX_SHADER};
    unique_ptr<char[]> _source;
    char _msg[MSG_SIZE] {};
};

NAME_SPACE_END();

#endif // MAIN_SHADER_H
