//
// Created by 常笑男 on 2024/2/29.
//

#ifndef MAIN_PROGRAM_H
#define MAIN_PROGRAM_H

#include <glad/glad.h>
#include <vector>

#include "glyph.h"
#include "shader.h"
#include "Util.h"
#include "vertex_array_obj.h"

NAME_SPACE_START(myUtil)

class Program {
public:
    using UniformId = GLint;

    Program() = default;
    template<typename... Args>
    Program(const std::string& filePath, Shader::ShaderType type, Args... args) {
        create(filePath, type, args...);
    }
    ~Program();

    void use() const;
    void push_back(std::unique_ptr<Shader>&& shader);
    bool linkProgram();
    UniformId uniform(const std::string& name);
    // uniform工具函数
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setUInt(const std::string& name, uint32_t value);
    void setFloat(const std::string& name, float value);
    void set4Float(const std::string& name, float x, float y, float z, float w);
    void setMatrix4fv(const std::string& name, const float* array);
    void setVec3fv(const std::string& name, const float* array);
    void setColor(const std::string& name, const Color& color);

    void renderGlyph(const std::u16string& context, const GlyphConfiguration& config, const VertexArrayObj& vao);

private:
    void create(const std::string& filePath, Shader::ShaderType type) { addShader(filePath, type); }
    template<typename... Args>
    void create(const std::string& filePath, Shader::ShaderType type, Args... args) {
        addShader(filePath, type);
        create(args...);
    }
    void addShader(const std::string& filePath, Shader::ShaderType type);

private:
    bool _status {true};
    uint32_t _programId {0};
    std::vector<std::unique_ptr<Shader>> _vertexShaderList;
    std::vector<std::unique_ptr<Shader>> _fragmentShaderList;
    std::unordered_map<std::string, UniformId> _uniformMap;
};

NAME_SPACE_END()

#endif // MAIN_PROGRAM_H
