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
using namespace std;

NAME_SPACE_START(myUtil)

class Program {
public:
    Program();
    template<typename... Args>
    Program(const char* filePath, Shader::ShaderType type, Args... args) {
        create(filePath, type, args...);
    }
    ~Program() = default;

    void use() const;
    void push_back(Shader* shader);
    bool linkProgram();
    void deleteProgram() const;
    // uniform工具函数
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setUInt(const std::string& name, uint32_t value) const;
    void setFloat(const std::string& name, float value) const;
    void set4Float(const std::string& name, float x, float y, float z, float w) const;
    void setMatrix4fv(const std::string& name, const float* array) const;
    void setVec3fv(const std::string& name, const float* array) const;
    void setColor(const std::string& name, const Color& color) const;

    void renderGlyph(const std::string& context, const GlyphConfiguration& config, const VertexArrayObj& vao) const;

private:
    void create(const char* filePath, Shader::ShaderType type);
    template<typename... Args>
    void create(const char* filePath, Shader::ShaderType type, Args... args) {
        if (_programId == 0) _programId = glCreateProgram();
        if (type == Shader::VERTEX_SHADER) {
            _vertexShaderList.push_back(new VertexShader(filePath));
        } else if (type == Shader::FRAGMENT_SHADER) {
            _fragmentShaderList.push_back(new FragmentShader(filePath));
        }
        create(args...);
    }

public:
    bool _status {true};
    uint32_t _programId {0};

private:
    vector<Shader*> _vertexShaderList;
    vector<Shader*> _fragmentShaderList;
};

NAME_SPACE_END()

#endif // MAIN_PROGRAM_H
