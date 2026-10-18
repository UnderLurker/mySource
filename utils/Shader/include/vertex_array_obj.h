//
// Created by 常笑男 on 2024/3/9.
//

#ifndef MAIN_VERTEX_ARRAY_OBJ_H
#define MAIN_VERTEX_ARRAY_OBJ_H

#include <glad/glad.h>
#include <list>
#include <map>
#include <set>

#include "buffer.h"
#include "enum.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

class Progrm;
struct VertexArrayInfo;

class VertexArrayObj final {
public:
    // <bufferId, buffer>
    using VBOMap = std::map<uint32_t, std::shared_ptr<Buffer>>;
    VertexArrayObj();
    ~VertexArrayObj();
    VertexArrayObj(const VertexArrayObj&)            = delete;
    VertexArrayObj(VertexArrayObj&&)                 = delete;
    VertexArrayObj& operator=(const VertexArrayObj&) = delete;
    VertexArrayObj& operator=(VertexArrayObj&&)      = delete;

    void pushBuffer(const std::shared_ptr<Buffer>& buffer);
    void eraseBuffer(uint32_t id);
    void eraseBuffer(const std::shared_ptr<Buffer>& buffer);
    std::shared_ptr<Buffer> getBufferById(uint32_t id);
    void bind() const { glBindVertexArray(_id); }
    void unBind() const { glBindVertexArray(0); }
    uint32_t id() const { return _id; }
    VBOMap getVBO() const { return _vbos; }
    std::shared_ptr<Buffer> getVEO() const { return _veo; }
    void setGlyphArrayBuffer();
    void updateGlyphArray(const std::vector<float>& data);
    void setVertexAttribPointer(const std::shared_ptr<Buffer>& buffer, const VertexArrayInfo& info);

private:
    bool hasBufferId(uint32_t id);

private:
    uint32_t _id {0};
    uint32_t _glyphId {0};
    VBOMap _vbos;
    std::shared_ptr<Buffer> _veo;
};

NAME_SPACE_END()

#endif // MAIN_VERTEX_ARRAY_OBJ_H
