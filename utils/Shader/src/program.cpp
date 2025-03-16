//
// Created by 常笑男 on 2024/2/29.
//
#include "program.h"

#include <glad/glad.h>

#include "shader_macro.h"

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
        char* msg = new char[MSG_SIZE];
        glGetProgramInfoLog(_programId, 512, nullptr, msg);
        // program link error
        LOGE("%s", msg);
        delete[] msg;
        return _status = false;
    }
    deleteShader(_vertexShaderList);
    deleteShader(_fragmentShaderList);
    return _status = true;
}

void Program::deleteProgram() const {
    int32_t success;
    glDeleteProgram(_programId);
    glGetProgramiv(_programId, GL_DELETE_STATUS, &success);
    if (!success) {
        char* msg = new char[MSG_SIZE];
        glGetProgramInfoLog(_programId, MSG_SIZE, nullptr, msg);
        // shader compiler delete failed
        LOGE("%s", msg);
        delete[] msg;
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

void Program::setMatrix4fv(const std::string& name, const float* array) const {
    glUniformMatrix4fv(glGetUniformLocation(_programId, name.c_str()), 1, GL_FALSE, array);
}

void Program::setVec3fv(const std::string& name, const float* array) const {
    glUniform3fv(glGetUniformLocation(_programId, name.c_str()), 1, array);
}

void Program::setColor(const std::string& name, const Color& color) const {
    auto tmp = color.convertFloat();
    if (tmp.size() != 4) return;
    set4Float(name, tmp[0], tmp[1], tmp[2], tmp[3]);
}

void Program::renderGlyph(const std::string& context,
                          const GlyphConfiguration& config,
                          const VertexArrayObj& vao) const {
    this->use();
    setColor("glyphColor", config.color);
    vao.use();
    float x = config.position.x, y = config.position.y;
    for (auto item : context) {
        Character ch = CharacterManager::GetInstance()->Get(item);

        GLfloat xpos = x + ch.bearing.x * config.scale;
        GLfloat ypos = y - (ch.size.height - ch.bearing.y) * config.scale;

        GLfloat w = ch.size.width * config.scale;
        GLfloat h = ch.size.height * config.scale;
        ch.texture->drawTexture(GL_TEXTURE0);
        vao.updateVertexData({xpos,     ypos + h, 0.0, 0.0,
                              xpos,     ypos,     0.0, 1.0,
                              xpos + w, ypos,     1.0, 1.0,
                              xpos,     ypos + h, 0.0, 0.0,
                              xpos + w, ypos,     1.0, 1.0,
                              xpos + w, ypos + h, 1.0, 0.0});
        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // 更新位置到下一个字形的原点，注意单位是1/64像素
        x += (ch.advance >> 6) * config.scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
    }
}

void Program::create(const char* filePath, Shader::ShaderType type) {
    if (_programId == 0) _programId = glCreateProgram();
    if (type == Shader::VERTEX_SHADER) {
        auto tmp = new VertexShader(filePath);
        if (tmp->_status) _vertexShaderList.push_back(tmp);
        else delete tmp;
    } else if (type == Shader::FRAGMENT_SHADER) {
        auto tmp = new FragmentShader(filePath);
        if (tmp->_status) _fragmentShaderList.push_back(tmp);
        else delete tmp;
    }
}
NAME_SPACE_END()
