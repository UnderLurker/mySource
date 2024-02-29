//
// Created by 常笑男 on 2024/2/29.
//
#include <glad/glad.h>
#include "program.h"
NAME_SPACE_START(myUtil)

Program::Program() {
    _programId = glCreateProgram();
}

Program::~Program() {
    int32_t success;
    glDeleteProgram(_programId);
    glGetProgramiv(_programId, GL_DELETE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_programId, MSG_SIZE, nullptr, _msg);
        cout << "shader compiler delete failed: " << _msg << endl;
    }
}

void Program::use() const {
    glUseProgram(_programId);
}

void Program::push_back(const Shader& shader) {
    if (!shader._status) return;
    _shaderList.push_back(shader._shaderId);
}

bool Program::linkProgram() {
    for (const auto& item : _shaderList) {
        glAttachShader(_programId, item);
    }
    glLinkProgram(_programId);
    int32_t success;
    glGetProgramiv(_programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_programId, 512, nullptr, _msg);
        cout << "program link error: " << _msg << endl;
        return _status = false;
    }
    return _status = true;
}

void Program::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(_programId, name.c_str()), (int)value);
}

void Program::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(_programId, name.c_str()), value);
}

void Program::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(_programId, name.c_str()), value);
}

void Program::set4Float(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(_programId, name.c_str()), x, y, z, w);
}
NAME_SPACE_END()