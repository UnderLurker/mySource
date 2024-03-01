//
// Created by 常笑男 on 2024/2/29.
//

#ifndef MAIN_PROGRAM_H
#define MAIN_PROGRAM_H

#include <vector>

#include "shader.h"
#include "Util.h"
using namespace std;

NAME_SPACE_START(myUtil)

class Program {
public:
    Program();
    ~Program() = default;

    void use() const;
    void push_back(const Shader& shader);
    bool linkProgram();
    void deleteProgram();
    // uniform工具函数
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setUInt(const std::string& name, uint32_t value) const;
    void setFloat(const std::string& name, float value) const;
    void set4Float(const std::string& name, float x, float y, float z, float w) const;

public:
    bool _status {true};
    uint32_t _programId {0};

private:
    vector<uint32_t> _shaderList;
    char _msg[MSG_SIZE] {};
};

NAME_SPACE_END()

#endif // MAIN_PROGRAM_H
