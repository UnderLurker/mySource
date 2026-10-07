//
// Created by Administrator on 2024/2/29.
//
#include "shader.h"

#include <glad/glad.h>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#include "include/logger_wrapper.h"

namespace {

std::string executableDir() {
#ifdef _WIN32
    char buf[MAX_PATH];
    const DWORD len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) return {};
    std::string path(buf, len);
    const auto pos = path.find_last_of("\\/");
    return pos == std::string::npos ? std::string {} : path.substr(0, pos);
#else
    char buf[PATH_MAX];
    const ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len == -1) return {};
    buf[len] = '\0';
    std::string path(buf, len);
    const auto pos = path.find_last_of('/');
    return pos == std::string::npos ? std::string {} : path.substr(0, pos);
#endif
}

bool isAbsolutePath(const std::string& path) {
    if (path.empty()) return false;
#ifdef _WIN32
    if (path[0] == '/' || path[0] == '\\') return true;
    return path.size() >= 3 && path[1] == ':';
#else
    return path[0] == '/';
#endif
}

// 相对路径解析到可执行文件所在目录；绝对路径原样返回
std::string resolvePath(const std::string& filePath) {
    if (isAbsolutePath(filePath)) return filePath;
    const std::string dir = executableDir();
    if (dir.empty()) return filePath;
    return dir + "/" + filePath;
}

} // namespace

NAME_SPACE_START(myUtil)

Shader::Shader(const std::string& filePath, ShaderType shaderType)
    : _shaderId(glCreateShader(shaderType)), _type(shaderType), _status(loadSource(filePath)) {}

Shader::Shader(const char* glsl, ShaderType shaderType)
    : _shaderId(glCreateShader(shaderType)), _type(shaderType), _status(compile(glsl)) {}

Shader::~Shader() {
    if (_shaderId == 0) return;
    int32_t success = 0;
    glDeleteShader(_shaderId);
    glGetShaderiv(_shaderId, GL_DELETE_STATUS, &success);
    if (!success) {
        int32_t len = 0;
        glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &len);
        std::string msg(len ? len + 1 : 1, '\0');
        glGetShaderInfoLog(_shaderId, len, nullptr, msg.data());
        // shader compiler delete failed
        LOGE("%s", msg.data());
    }
}

bool Shader::loadSource(const std::string& filePath) {
    const std::string resolvedPath = resolvePath(filePath);
    stringstream ss;
    try {
        std::fstream file(resolvedPath, std::ios::in | std::ios::binary);
        if (file.fail()) {
            LOGE("[Shader] loadSource open fail: path=[%s] len=%lu", resolvedPath.c_str(),
                 (unsigned long)resolvedPath.size());
            return false;
        }
        ss << file.rdbuf();
    } catch (ifstream::failure& e) {
        LOGE("[Shader] loadSource failed: %s", e.what());
        return false;
    } catch (...) {
        LOGE("[Shader] loadSource failed, unknown.");
        return false;
    }
    return compile(ss.str(), resolvedPath);
}

bool Shader::compile(const string& source, const std::string& filePath) {
    int32_t success     = 0;
    const char* _source = source.c_str();
    glShaderSource(_shaderId, 1, &_source, nullptr);
    glCompileShader(_shaderId);
    glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        int32_t len = 0;
        glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &len);
        std::string msg(len ? len + 1 : 1, '\0');
        glGetShaderInfoLog(_shaderId, len, nullptr, msg.data());
        // shader compile error
        LOGE("filePath: %s msg: %s", filePath.c_str(), msg.data());
        return false;
    }
    return true;
}

bool Shader::compile(const char* source) {
    std::string str;
    str.assign(source);
    return compile(str, "string");
}
NAME_SPACE_END()
