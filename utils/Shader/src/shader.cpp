//
// Created by Administrator on 2024/2/29.
//
#include "shader.h"

#include <glad/glad.h>
#include <sstream>

#include "include/logger_wrapper.h"

NAME_SPACE_START(myUtil)

Shader::Shader(const std::string& filePath, ShaderType shaderType)
    : _shaderId(glCreateShader(shaderType)), _type(shaderType), _status(loadSource(filePath)) {}

Shader::Shader(const char* glsl, ShaderType shaderType)
    : _shaderId(glCreateShader(shaderType)), _type(shaderType), _status(compile(glsl)) {}

Shader::~Shader() {
    if (_shaderId == 0) return;
    int32_t success = 0;
    glDeleteShader(_shaderId);
    glGetShaderiv(_shaderId, GL_DELETE_STATUS, &success);
    if (!success) {
        int32_t len = 0;
        glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &len);
        std::string msg(len ? len + 1 : 1, '\0');
        glGetShaderInfoLog(_shaderId, len, nullptr, msg.data());
        // shader compiler delete failed
        LOGE("%s", msg.data());
    }
}

bool Shader::loadSource(const std::string& filePath) {
    stringstream ss;
    try {
        std::fstream file(filePath, std::ios::in | std::ios::binary);
        if (file.fail()) return false;
        ss << file.rdbuf();
    } catch (ifstream::failure& e) {
        LOGE("loadSource failed: %s", e.what());
        return false;
    }
    return compile(ss.str(), filePath);
}

bool Shader::compile(const string& source, const std::string& filePath) {
    int32_t success     = 0;
    const char* _source = source.c_str();
    glShaderSource(_shaderId, 1, &_source, nullptr);
    glCompileShader(_shaderId);
    glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        int32_t len = 0;
        glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &len);
        std::string msg(len ? len + 1 : 1, '\0');
        glGetShaderInfoLog(_shaderId, len, nullptr, msg.data());
        // shader compile error
        LOGE("filePath: %s msg: %s", filePath.c_str(), msg.data());
        return false;
    }
    return true;
}

bool Shader::compile(const char* source) {
    std::string str;
    str.assign(source);
    return compile(str, "string");
}
NAME_SPACE_END()
