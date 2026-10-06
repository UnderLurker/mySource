//
// Created by cxn on 2026/10/06.
// 2D 渲染后端：一个 shader 通吃纯色/纹理，批处理，正交投影（左上角原点）。
//

#ifndef _RENDERER_H
#define _RENDERER_H

#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "coord.h"
#include "program.h"
#include "shader.h"
#include "ULGuiColor.h"

namespace ULGui::base {

class Renderer {
public:
    static Renderer& instance();

    /**
     * 初始化 GL 资源：加载 glad、编译 shader、链接 program、创建 VAO/VBO 与 1x1 白纹理。
     * 必须在 GL


     * * * 上下文就绪（glfwMakeContextCurrent 之后）调用。
     * @param getProcAddress 由调用方传入
     *
     *
     * glfwGetProcAddress，用于加载 GL 函数指针。
     */
    bool init(int viewportWidth, int viewportHeight, void* (*getProcAddress)(const char*));
    void shutdown();

    // 更新视口尺寸并重算正交投影（窗口尺寸变化时调用）
    void setViewport(int width, int height);

    // 清屏
    void clear(const RGBA& color);

    // —— 图元接口：只往批处理缓冲区塞顶点，不真正绘制 ——
    void drawRect(float x, float y, float w, float h, const RGBA& color);
    void drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, const RGBA& color);
    void drawLine(float x0, float y0, float x1, float y1, float width, const RGBA& color);
    void drawCircle(float cx, float cy, float r, const RGBA& color, bool fill, float width);
    void drawArc(float cx, float cy, float r, float startDeg, float endDeg, float width, const RGBA& color);
    void drawPoint(float x, float y, float size, const RGBA& color);

    // 一次性上传所有顶点并绘制，然后清空缓冲区
    void flush();

private:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer&)            = delete;
    Renderer(Renderer&&)                 = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&)      = delete;

    // 顶点格式：pos(2) + color(4) + uv(2)
    void pushVertex(float x, float y, const RGBA& c, float u, float v);
    void pushTriangle(float x0, float y0, float x1, float y1, float x2, float y2, const RGBA& c);
    // 圆环扇区：内半径 inner、外半径 outer、角度范围 [startRad, endRad]，拆成三角形
    void pushArc(float cx, float cy, float inner, float outer, float startRad, float endRad, const RGBA& c);

private:
    std::unique_ptr<myUtil::Program> _program;
    std::unique_ptr<myUtil::Texture> _texture;
    std::unique_ptr<myUtil::VertexArrayObj> _vao;

    std::vector<float> _buffer;
    size_t _vertexCount {0};

    int _viewportW {0};
    int _viewportH {0};
    glm::mat4 _projection {1.0f};

    bool _inited {false};
};

} // namespace ULGui::base

#endif // _RENDERER_H
