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
    void deleteShader() const;
private:
    bool loadSource(const std::string& filePath, ShaderType shaderType);
    bool compile(const string& source);

public:
    bool _status {true};
    uint32_t _shaderId {0};
    ShaderType _type {VERTEX_SHADER};
};

NAME_SPACE_END();

#endif // MAIN_SHADER_H
