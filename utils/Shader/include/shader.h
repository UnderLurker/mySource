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
    enum ShaderType {
        FRAGMENT_SHADER = 0x8B30,
        VERTEX_SHADER   = 0x8B31
    };
    Shader()            = delete;
    Shader(Shader& obj) = delete;
    Shader(Shader&& obj) noexcept;
    explicit Shader(const char* filePath, ShaderType shaderType);
    virtual ~Shader() = default;
    void deleteShader() const;

private:
    bool loadSource(const std::string& filePath, ShaderType shaderType);
    bool compile(const string& source, const std::string& filePath);

public:
    bool _status {true};
    uint32_t _shaderId {0};
    ShaderType _type {VERTEX_SHADER};
};

class VertexShader : public virtual Shader {
public:
    VertexShader() = delete;
    explicit VertexShader(const char* filePath)
        : Shader(filePath, VERTEX_SHADER) {}
    ~VertexShader() override = default;
};

class FragmentShader : public virtual Shader {
public:
    FragmentShader() = delete;
    explicit FragmentShader(const char* filePath)
        : Shader(filePath, FRAGMENT_SHADER) {}
    ~FragmentShader() override = default;
};
NAME_SPACE_END();

#endif // MAIN_SHADER_H
