//
// Created by 常笑男 on 2024/2/29.
//
#include "program.h"

#include <glad/glad.h>

#include "include/logger_wrapper.h"
#include "shader_macro.h"

NAME_SPACE_START(myUtil)
void shaderAttach(uint32_t programId, const std::vector<std::unique_ptr<Shader>>& shaderList) {
    for (const auto& item : shaderList)
        if (item && item->_shaderId) glAttachShader(programId, item->_shaderId);
}

Program::~Program() {
    if (_programId == 0 || !_status) return;
    int32_t success = 0;
    glDeleteProgram(_programId);
    glGetProgramiv(_programId, GL_DELETE_STATUS, &success);
    if (!success) {
        int32_t len = 0;
        glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &len);
        std::string msg(len ? len + 1 : 1, '\0');
        glGetProgramInfoLog(_programId, len, nullptr, msg.data());
        // shader compiler delete failed
        LOGE("%s", msg.data());
    }
    _programId = 0;
}

void Program::use() const { glUseProgram(_programId); }

void Program::push_back(std::unique_ptr<Shader>&& shader) {
    if (!shader->_status) return;
    if (shader->_type == Shader::VERTEX_SHADER) _vertexShaderList.push_back(std::move(shader));
    else if (shader->_type == Shader::FRAGMENT_SHADER) _fragmentShaderList.push_back(std::move(shader));
}

bool Program::linkProgram() {
    if (_programId == 0) {
        _programId = glCreateProgram();
        if (_programId == 0) return false;
    }
    if (_vertexShaderList.empty() || _fragmentShaderList.empty()) return false;
    shaderAttach(_programId, _vertexShaderList);
    shaderAttach(_programId, _fragmentShaderList);
    glLinkProgram(_programId);
    int32_t success = 0;
    glGetProgramiv(_programId, GL_LINK_STATUS, &success);
    if (!success) {
        int32_t len = 0;
        glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &len);
        std::string msg(len ? len + 1 : 1, '\0');
        glGetProgramInfoLog(_programId, len, nullptr, msg.data());
        // program link error
        LOGE("%s", msg.data());
        return _status = false;
    }
    _vertexShaderList.clear();
    _fragmentShaderList.clear();
    return _status = true;
}

Program::UniformId Program::uniform(const std::string& name) {
    if (_uniformMap.find(name) != _uniformMap.end()) return _uniformMap[name];
    UniformId t = glGetUniformLocation(_programId, name.c_str());
    if (t < 0) {
        LOGE("Program get uniform location fail, name: %s", name.c_str());
        return -1;
    }
    _uniformMap[name] = t;
    return t;
}

void Program::setBool(const std::string& name, bool value) {
    auto id = uniform(name);
    if (id < 0) {
        LOGW("[Program] setBool fail, name: %s", name.c_str());
        return;
    }
    glUniform1i(id, (int)value);
}

void Program::setInt(const std::string& name, int value) {
    auto id = uniform(name);
    if (id < 0) {
        LOGW("[Program] setInt fail, name: %s", name.c_str());
        return;
    }
    glUniform1i(id, value);
}

void Program::setUInt(const std::string& name, uint32_t value) {
    auto id = uniform(name);
    if (id < 0) {
        LOGW("[Program] setUInt fail, name: %s", name.c_str());
        return;
    }
    glUniform1ui(id, value);
}

void Program::setFloat(const std::string& name, float value) {
    auto id = uniform(name);
    if (id < 0) {
        LOGW("[Program] setFloat fail, name: %s", name.c_str());
        return;
    }
    glUniform1f(id, value);
}

void Program::set4Float(const std::string& name, float x, float y, float z, float w) {
    auto id = uniform(name);
    if (id < 0) {
        LOGW("[Program] set4Float fail, name: %s", name.c_str());
        return;
    }
    glUniform4f(id, x, y, z, w);
}

void Program::setMatrix4fv(const std::string& name, const float* array) {
    auto id = uniform(name);
    if (id < 0) {
        LOGW("[Program] setMatrix4fv fail, name: %s", name.c_str());
        return;
    }
    glUniformMatrix4fv(id, 1, GL_FALSE, array);
}

void Program::setVec3fv(const std::string& name, const float* array) {
    auto id = uniform(name);
    if (id < 0) {
        LOGW("[Program] setVec3fv fail, name: %s", name.c_str());
        return;
    }
    glUniform3fv(id, 1, array);
}

void Program::setColor(const std::string& name, const Color& color) {
    auto tmp = color.convertFloat();
    if (tmp.size() != 4) return;
    set4Float(name, tmp[0], tmp[1], tmp[2], tmp[3]);
}

void Program::renderGlyph(const std::u16string& context,
                          const GlyphConfiguration& config,
                          const VertexArrayObj& vao) {
    this->use();
    setColor("glyphColor", config.color);
    vao.use();
    float x = config.position.x, y = config.position.y;
    auto codePoints = FontManager::UTF16ToCodePoints(context);
    for (auto item : codePoints) {
        Character ch = FontManager::GetInstance()->Get(item);
        if (!ch.texture) continue;

        GLfloat xPos = x + ch.bearing.x * config.scale;
        GLfloat yPos = y - (ch.size.height - ch.bearing.y) * config.scale;

        GLfloat w = ch.size.width * config.scale;
        GLfloat h = ch.size.height * config.scale;
        ch.texture->drawTexture(GL_TEXTURE0);
        vao.updateData({xPos,     yPos + h, 0.0, 0.0,
                        xPos,     yPos,     0.0, 1.0,
                        xPos + w, yPos,     1.0, 1.0,
                        xPos,     yPos + h, 0.0, 0.0,
                        xPos + w, yPos,     1.0, 1.0,
                        xPos + w, yPos + h, 1.0, 0.0});
        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // 更新位置到下一个字形的原点，注意单位是1/64像素
        x += (ch.advance >> 6) * config.scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
    }
}

void Program::addShader(const std::string& filePath, Shader::ShaderType type) {
    if (type == Shader::VERTEX_SHADER) {
        auto shader = std::make_unique<VertexShader>(filePath);
        if (shader->_status) _vertexShaderList.push_back(std::move(shader));
    } else if (type == Shader::FRAGMENT_SHADER) {
        auto shader = std::make_unique<FragmentShader>(filePath);
        if (shader->_status) _fragmentShaderList.push_back(std::move(shader));
    }
}
NAME_SPACE_END()
