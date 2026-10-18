//
// Created by cxn on 2026/10/18.
//
#include "triangleBatchDrawer.h"

#include <glm/gtc/type_ptr.hpp>

#include "include/logger_wrapper.h"
#include "roundedRectDrawer.h" // uploadBrushUniforms（画刷 uniform 上传共用）

namespace ULGui::base {
namespace {
const std::string TRI_VERT_PATH = "base/shader/renderer.vert";
const std::string TRI_FRAG_PATH = "base/shader/renderer.frag";
} // namespace

TriangleBatchDrawer::~TriangleBatchDrawer() { shutdown(); }

bool TriangleBatchDrawer::init() {
    if (_inited) return true;
    _program = std::make_unique<myUtil::Program>();
    if (!_program) {
        LOGE("[TriangleBatchDrawer] create Program fail.");
        return false;
    }
    _program->setName("TriangleBatchDrawer");
    std::unique_ptr<myUtil::Shader> vShader = std::make_unique<myUtil::VertexShader>(TRI_VERT_PATH);
    std::unique_ptr<myUtil::Shader> fShader = std::make_unique<myUtil::FragmentShader>(TRI_FRAG_PATH);
    if (!vShader || !fShader) return false;
    _program->push_back(std::move(vShader));
    _program->push_back(std::move(fShader));
    if (!_program->linkProgram()) {
        LOGE("[TriangleBatchDrawer] linkProgram fail!!!");
        return false;
    }

    // 1x1 白纹理：让纯色和纹理共用同一个 shader
    unsigned char white[4] = {255, 255, 255, 255};
    _texture               = std::make_unique<myUtil::Texture>();
    if (!_texture) {
        LOGE("[TriangleBatchDrawer] create Texture fail!!!");
        return false;
    }
    _texture->use();
    _texture->setImage2D(1, 1, white);
    _texture->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR, GL_TEXTURE_MAG_FILTER, GL_LINEAR, GL_TEXTURE_WRAP_S,
                       GL_CLAMP_TO_EDGE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _vertexBuffer = std::make_shared<myUtil::BufferF>(myUtil::BufferType::VBO);
    _vao          = std::make_unique<myUtil::VertexArrayObj>();
    if (!_vertexBuffer || !_vao) {
        LOGE("[TriangleBatchDrawer] create buffer/vao fail.");
        return false;
    }
    if (!createVAO()) return false;
    _vertices.reserve(static_cast<size_t>(kMaxVertices) * kFloatsPerVertex);
    _inited = true;
    return true;
}

void TriangleBatchDrawer::shutdown() {
    _vertices.clear();
    _vertexCount = 0;
    _vertexBuffer.reset();
    _vao.reset();
    _texture.reset();
    _program.reset();
    _inited = false;
}

bool TriangleBatchDrawer::createVAO() {
    // 交错布局：pos(2) + color(4) + uv(2)，预留整帧容量，flush 时 glBufferSubData
    _vertexBuffer->setData(static_cast<int64_t>(kMaxVertices) * kFloatsPerVertex, myUtil::BufferUsage::DYNAMIC_DRAW);
    _vao->setVertexAttribPointer(_vertexBuffer, {.attrIndex = 0, .size = 2, .stride = 8, .offset = 0});
    _vao->setVertexAttribPointer(_vertexBuffer, {.attrIndex = 1, .size = 4, .stride = 8, .offset = 2});
    _vao->setVertexAttribPointer(_vertexBuffer, {.attrIndex = 2, .size = 2, .stride = 8, .offset = 6});
    return true;
}

void TriangleBatchDrawer::pushVertex(float x, float y, const RGBA& c, float u, float v) {
    _vertices.push_back(x);
    _vertices.push_back(y);
    _vertices.push_back(static_cast<float>(c.red()) / 255.0f);
    _vertices.push_back(static_cast<float>(c.green()) / 255.0f);
    _vertices.push_back(static_cast<float>(c.blue()) / 255.0f);
    _vertices.push_back(static_cast<float>(c.alpha()) / 255.0f);
    _vertices.push_back(u);
    _vertices.push_back(v);
    ++_vertexCount;
}

void TriangleBatchDrawer::updateBuffer() {
    if (!_inited || _vertexCount == 0) return;
    _vertexBuffer->update(_vertices);
}

void TriangleBatchDrawer::drawRange(int32_t first, int32_t count, const glm::mat4& projection, const ULBrush& brush) {
    if (!_inited || count == 0) return;
    _program->bind();
    _program->setMatrix4fv("projection", glm::value_ptr(projection));
    _program->setInt("tex", 0);
    uploadBrushUniforms(*_program, brush);
    _texture->drawTexture(GL_TEXTURE0);
    _vao->bind();
    glDrawArrays(GL_TRIANGLES, first, count);
}

void TriangleBatchDrawer::clear() {
    _vertices.clear();
    _vertexCount = 0;
}

} // namespace ULGui::base
