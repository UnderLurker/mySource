//
// Created by Administrator on 2024/2/29.
//
#include "shader.h"

#include <glad/glad.h>
#include <sstream>

NAME_SPACE_START(myUtil)

Shader::Shader(const char* filePath, ShaderType shaderType) {
    _shaderId = glCreateShader(shaderType);
    _status = loadSource(filePath, shaderType);
    _type   = shaderType;
//    if (_status) deleteShader();
}

bool Shader::loadSource(const std::string& filePath, ShaderType shaderType) {
    string _source;
    try {
        std::fstream file(filePath, std::ios::in | std::ios::binary);
        if (file.fail()) return false;
        stringstream ss;
        ss << file.rdbuf();
        _source = ss.str();
    } catch (ifstream::failure& e) {
        cout << "loadSource failed: " << e.what() << endl;
        return false;
    }
    return compile(_source);
}

bool Shader::compile(const string& source) {
    int32_t success;
    char _msg[MSG_SIZE];
    const char* _source = source.c_str();
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

void Shader::deleteShader() const {
    int32_t success;
    char _msg[MSG_SIZE];
    glDeleteShader(_shaderId);
    glGetShaderiv(_shaderId, GL_DELETE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(_shaderId, MSG_SIZE, nullptr, _msg);
        cout << "shader compiler delete failed: " << _msg << endl;
    }
}

NAME_SPACE_END()
