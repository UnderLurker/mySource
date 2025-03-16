//
// Created by Administrator on 2024/2/29.
//
#include "shader.h"

#include <glad/glad.h>
#include <sstream>

#include "shader_macro.h"

NAME_SPACE_START(myUtil)

Shader::Shader(const char* filePath, ShaderType shaderType) {
    _shaderId = glCreateShader(shaderType);
    _status   = loadSource(filePath, shaderType);
    _type     = shaderType;
    //    if (_status) deleteShader();
}

Shader::Shader(Shader&& obj) noexcept {
    _status       = obj._status;
    _shaderId     = obj._shaderId;
    _type         = obj._type;
    obj._shaderId = 0;
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
    return compile(_source, filePath);
}

bool Shader::compile(const string& source, const std::string& filePath) {
    int32_t success;
    const char* _source = source.c_str();
    glShaderSource(_shaderId, 1, &_source, nullptr);
    glCompileShader(_shaderId);
    glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        char* msg = new char[MSG_SIZE];
        glGetShaderInfoLog(_shaderId, MSG_SIZE, nullptr, msg);
        // shader compile error
        LOGE("filePath: %s msg: %s", filePath.c_str(), msg);
        delete[] msg;
        _status = false;
        return false;
    }
    return true;
}

void Shader::deleteShader() const {
    if (_shaderId == 0 || !_status) return;
    int32_t success;
    glDeleteShader(_shaderId);
    glGetShaderiv(_shaderId, GL_DELETE_STATUS, &success);
    if (!success) {
        char* msg = new char[MSG_SIZE];
        glGetShaderInfoLog(_shaderId, MSG_SIZE, nullptr, msg);
        // shader compiler delete failed
        LOGE("%s", msg);
        delete[] msg;
    }
}

NAME_SPACE_END()
