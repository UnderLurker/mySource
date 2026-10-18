//
// Created by 常笑男 on 2024/3/9.
//
#include "vertex_array_obj.h"

#include "common.h"
#include "include/logger_wrapper.h"

NAME_SPACE_START(myUtil)

VertexArrayObj::VertexArrayObj() { glGenVertexArrays(1, &_id); }
VertexArrayObj::~VertexArrayObj() {
    if (_id != 0) glDeleteVertexArrays(1, &_id);
}

void VertexArrayObj::pushBuffer(const std::shared_ptr<Buffer>& buffer) {
    if (!buffer || hasBufferId(buffer->id())) return;
    if (buffer->type() == BufferType::VBO) _vbos[buffer->id()] = buffer;
    else if (buffer->type() == BufferType::VEO) _veo = buffer;
    else { LOGW("don't push buffer type UBO to VAO!!!"); }
}

std::shared_ptr<Buffer> VertexArrayObj::getBufferById(uint32_t id) {
    if (_veo && _veo->id() == id) return _veo;
    if (!hasBufferId(id)) return nullptr;
    return _vbos[id];
}

void VertexArrayObj::eraseBuffer(uint32_t id) {
    if (_veo && _veo->id() == id) _veo.reset();
    if (!hasBufferId(id)) return;
    _vbos.erase(id);
}

void VertexArrayObj::eraseBuffer(const std::shared_ptr<Buffer>& buffer) {
    if (!buffer) return;
    if (_veo == buffer) _veo.reset();
    else if (buffer->type() == BufferType::VBO) _vbos.erase(buffer->id());
}

void VertexArrayObj::setGlyphArrayBuffer() {
    bind();
    if (_glyphId != 0) return;
    auto buffer = std::make_shared<BufferF>(BufferType::VBO);
    buffer->setData(4 * 6, BufferUsage::DYNAMIC_DRAW);
    _glyphId = buffer->id();
    VertexArrayInfo info;
    info.attrIndex = 0;
    info.size      = 4;
    info.stride    = 4;
    info.offset    = 0;
    setVertexAttribPointer(buffer, info);
}

void VertexArrayObj::updateGlyphArray(const std::vector<float>& data) {
    if (_glyphId == 0 || !hasBufferId(_glyphId)) return;
    if (_vbos.at(_glyphId)) _vbos.at(_glyphId)->update(data);
}

void VertexArrayObj::setVertexAttribPointer(const std::shared_ptr<Buffer>& buffer, const VertexArrayInfo& info) {
    if (!buffer) return;
    if (!hasBufferId(buffer->id())) pushBuffer(buffer);
    bind();
    buffer->setVertexAttr(info.attrIndex, info.size, info.stride, info.offset, info.divisor, info.normalized);
}

bool VertexArrayObj::hasBufferId(uint32_t id) { return _vbos.find(id) != _vbos.end(); }

NAME_SPACE_END()
