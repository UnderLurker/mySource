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
    explicit Shader(const std::string& filePath, ShaderType shaderType);
    explicit Shader(const char* glsl, ShaderType shaderType);
    virtual ~Shader();

private:
    bool loadSource(const std::string& filePath);
    bool compile(const string& source, const std::string& filePath);
    bool compile(const char* source);
    Shader()                         = delete;
    Shader(const Shader&)            = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&)      = delete;

public:
    uint32_t _shaderId {0};
    ShaderType _type {VERTEX_SHADER};
    bool _status {true};
};

class VertexShader : public virtual Shader {
public:
    explicit VertexShader(const std::string& filePath)
        : Shader(filePath, VERTEX_SHADER) {}
    explicit VertexShader(const char* glsl)
        : Shader(glsl, VERTEX_SHADER) {}
    ~VertexShader() override = default;

private:
    VertexShader()                               = delete;
    VertexShader(const VertexShader&)            = delete;
    VertexShader(VertexShader&&)                 = delete;
    VertexShader& operator=(const VertexShader&) = delete;
    VertexShader& operator=(VertexShader&&)      = delete;
};

class FragmentShader : public virtual Shader {
public:
    explicit FragmentShader(const std::string& filePath)
        : Shader(filePath, FRAGMENT_SHADER) {}
    explicit FragmentShader(const char* glsl)
        : Shader(glsl, FRAGMENT_SHADER) {}
    ~FragmentShader() override = default;

private:
    FragmentShader()                                 = delete;
    FragmentShader(const FragmentShader&)            = delete;
    FragmentShader(FragmentShader&&)                 = delete;
    FragmentShader& operator=(const FragmentShader&) = delete;
    FragmentShader& operator=(FragmentShader&&)      = delete;
};
NAME_SPACE_END();

#endif // MAIN_SHADER_H
