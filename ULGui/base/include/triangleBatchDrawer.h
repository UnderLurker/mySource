//
// Created by cxn on 2026/10/18.
// 主批次三角形绘制器：一个 program 通吃纯色/渐变，批处理顶点，flush 时一次 glDrawArrays。
//

#ifndef _TRIANGLE_BATCH_DRAWER_H
#define _TRIANGLE_BATCH_DRAWER_H

#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "brush.h"
#include "program.h"
#include "texture.h"
#include "vertex_array_obj.h"

namespace ULGui::base {

/**
 * 主批次三角形绘制器：矩形/线段/圆/弧等图元都拆成三角形塞进这里。
 * 顶点格式（8 floats）：pos(2) + color(4) + uv(2)。
 * 段/命令列表由 Renderer 持有，本类只负责顶点暂存与绘制。
 */
class TriangleBatchDrawer {
public:
    static constexpr uint32_t kMaxVertices     = 1u << 16; // 65536 顶点，单帧容量
    static constexpr uint32_t kFloatsPerVertex = 8;

    TriangleBatchDrawer() = default;
    ~TriangleBatchDrawer();
    TriangleBatchDrawer(const TriangleBatchDrawer&)            = delete;
    TriangleBatchDrawer(TriangleBatchDrawer&&)                 = delete;
    TriangleBatchDrawer& operator=(const TriangleBatchDrawer&) = delete;
    TriangleBatchDrawer& operator=(TriangleBatchDrawer&&)      = delete;

    // 编译 renderer.vert/.frag、创建 1x1 白纹理与动态 VBO/VAO；GL 上下文就绪后调用
    bool init();
    void shutdown();
    void pushVertex(float x, float y, const RGBA& c, float u, float v);
    bool isFull() const { return _vertexCount >= kMaxVertices; }
    uint32_t vertexCount() const { return _vertexCount; }
    void updateBuffer();
    // 绘制顶点区间 [first, first+count)：bind program、上传 projection/tex/brush uniform、
    // bind 白纹理与 VAO、glDrawArrays
    void drawRange(int32_t first, int32_t count, const glm::mat4& projection, const ULBrush& brush);
    // 清空顶点暂存（每帧 flush 结束后由 Renderer 调用）
    void clear();

private:
    bool createVAO();

    std::unique_ptr<myUtil::Program> _program;
    std::unique_ptr<myUtil::Texture> _texture; // 1x1 白纹理：纯色与纹理共用同一 shader
    std::unique_ptr<myUtil::VertexArrayObj> _vao;
    std::shared_ptr<myUtil::BufferF> _vertexBuffer; // 动态 VBO
    std::vector<float> _vertices;                   // CPU 暂存
    uint32_t _vertexCount {0};
    bool _inited {false};
};

} // namespace ULGui::base

#endif // _TRIANGLE_BATCH_DRAWER_H
