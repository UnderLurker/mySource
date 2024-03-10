//
// Created by 常笑男 on 2024/2/29.
//
#include "program.h"

#include <glad/glad.h>
NAME_SPACE_START(myUtil)

void shaderAttach(uint32_t programId, const vector<Shader*>& shaderList) {
    for (const auto& item : shaderList)
        glAttachShader(programId, item->_shaderId);
}

void deleteShader(vector<Shader*>& shaderList) {
    for (auto& item : shaderList) {
        item->deleteShader();
        delete item;
        item = nullptr;
    }
}

Program::Program() { _programId = glCreateProgram(); }

void Program::use() const { glUseProgram(_programId); }

void Program::push_back(Shader* shader) {
    if (!shader->_status) return;
    if (shader->_type == Shader::VERTEX_SHADER) _vertexShaderList.push_back(shader);
    else if (shader->_type == Shader::FRAGMENT_SHADER) _fragmentShaderList.push_back(shader);
}

bool Program::linkProgram() {
    if (_vertexShaderList.empty() || _fragmentShaderList.empty()) return false;
    shaderAttach(_programId, _vertexShaderList);
    shaderAttach(_programId, _fragmentShaderList);
    glLinkProgram(_programId);
    int32_t success;
    glGetProgramiv(_programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_programId, 512, nullptr, _msg);
        cout << "program link error: " << _msg << endl;
        return _status = false;
    }
    deleteShader(_vertexShaderList);
    deleteShader(_fragmentShaderList);
    return _status = true;
}

void Program::deleteProgram() {
    int32_t success;
    glDeleteProgram(_programId);
    glGetProgramiv(_programId, GL_DELETE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_programId, MSG_SIZE, nullptr, _msg);
        cout << "shader compiler delete failed: " << _msg << endl;
    }
}

void Program::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(_programId, name.c_str()), (int)value);
}

void Program::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(_programId, name.c_str()), value);
}

void Program::setUInt(const std::string& name, uint32_t value) const {
    glUniform1ui(glGetUniformLocation(_programId, name.c_str()), value);
}

void Program::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(_programId, name.c_str()), value);
}

void Program::set4Float(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(_programId, name.c_str()), x, y, z, w);
}

void Program::setMatrix4fv(const std::string& name, float* array) const {
    glUniformMatrix4fv(glGetUniformLocation(_programId, name.c_str()), 1, GL_FALSE, array);
}

void Program::create(const char* filePath, Shader::ShaderType type) {
    if (_programId == 0) _programId = glCreateProgram();
    if (type == Shader::VERTEX_SHADER) {
        _vertexShaderList.push_back(new VertexShader(filePath));
    } else if (type == Shader::FRAGMENT_SHADER) {
        _fragmentShaderList.push_back(new FragmentShader(filePath));
    }
}
NAME_SPACE_END()
