//
// Created by Administrator on 2024/2/29.
//
#include "shader.h"

#include <glad/glad.h>

NAME_SPACE_START(myUtil)

Shader::Shader(const char* filePath, ShaderType shaderType) {
    createShader(shaderType);
    _status = loadSource(filePath);
    _type   = shaderType;
    compile();
}

Shader::~Shader() {
    int32_t success;
    delete[] _source;
    _source = nullptr;
    glDeleteShader(_shaderId);
    glGetShaderiv(_shaderId, GL_DELETE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(_shaderId, MSG_SIZE, nullptr, _msg);
        cout << "shader compiler delete failed: " << _msg << endl;
    }
}

bool Shader::loadSource(const std::string& filePath) {
    delete[] _source;
    _source = nullptr;

    std::fstream file(filePath, std::ios::in | std::ios::binary);
    if (file.fail()) return false;
    file.seekg(0, std::fstream::end);
    size_t endPos = file.tellg();
    file.seekg(0, std::fstream::beg);
    _source = new char[endPos + 1];
    file.read(_source, static_cast<long long>(endPos));
    _source[endPos] = '\0';
    return true;
}

void Shader::createShader(ShaderType shaderType) {
    _shaderId = glCreateShader(shaderType);
}

bool Shader::compile() {
    int32_t success;
    glShaderSource(_shaderId, 1, &_source, nullptr);
    glCompileShader(_shaderId);
    glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(_shaderId, MSG_SIZE, nullptr, const_cast<char*>(_msg));
        cout << "shader compile error: " << _msg << endl;
        _status = false;
        return false;
    }
    return true;
}

NAME_SPACE_END()