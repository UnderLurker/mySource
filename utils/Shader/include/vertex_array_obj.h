//
// Created by 常笑男 on 2024/3/9.
//

#ifndef MAIN_VERTEX_ARRAY_OBJ_H
#define MAIN_VERTEX_ARRAY_OBJ_H

#include <glad/glad.h>
#include <map>

#include "Util.h"

NAME_SPACE_START(myUtil)

class Progrm;

enum DataType {
    BYTE           = 0x1400,
    UNSIGNED_BYTE  = 0x1401,
    SHORT          = 0x1402,
    UNSIGNED_SHORT = 0x1403,
    INT            = 0x1404,
    UNSIGNED_INT   = 0x1405,
    FLOAT          = 0x1406
};

class VertexArrayObj {
public:
    enum BufferUsage {
        STREAM_DRAW  = 0x88E0,
        STREAM_READ  = 0x88E1,
        STREAM_COPY  = 0x88E2,
        STATIC_DRAW  = 0x88E4,
        STATIC_READ  = 0x88E5,
        STATIC_COPY  = 0x88E6,
        DYNAMIC_DRAW = 0x88E8,
        DYNAMIC_READ = 0x88E9,
        DYNAMIC_COPY = 0x88EA
    };
    enum BufferType {
        VBO,
        VEO,
    };
    VertexArrayObj() { glGenVertexArrays(1, &_id); }
    virtual ~VertexArrayObj() { reset(); }

    void reset() {
        resetVBO();
        resetVEO();
        resetVAO();
    }
    void resetVAO() { if (_id != 0) glDeleteVertexArrays(1, &_id); }
    void resetVBO() {
        if (_vertexBufferObj != 0) {
            glDeleteBuffers(1, &_vertexBufferObj);
            _vboIndexMap.clear();
        }
    }
    void resetVEO() { if (_vertexEleObj != 0) glDeleteBuffers(1, &_vertexEleObj); }

    template<typename T>
    void setArrayBuffer(const vector<T>& data, BufferUsage usage) {
        resetVBO();
        glGenBuffers(1, &_vertexBufferObj);
        glBindVertexArray(_id);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObj);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), usage);
    }
    template<typename T>
    void setArrayBuffer(size_t size, BufferUsage usage) {
        resetVBO();
        glGenBuffers(1, &_vertexBufferObj);
        glBindVertexArray(_id);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObj);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * size, nullptr, usage);
    }
    void setGlyphArrayBuffer() {
        setArrayBuffer<float>(4 * 6, myUtil::VertexArrayObj::DYNAMIC_DRAW);
        setVertexAttribPointer<float>(4, myUtil::FLOAT, 4, 0);
    }

    void setEleArrayBuffer(const vector<uint32_t>& data, BufferUsage usage);

    template<typename T>
    void
        setVertexAttribPointer(int32_t size, DataType type, uint32_t stride, uint32_t offset, bool normalized = false) {
        glBindVertexArray(_id);
        uint32_t index = _vboIndexMap.size();
        glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, sizeof(T) * stride,
                              (void*)(sizeof(T) * offset));
        _vboIndexMap[index] = false;
        enableVertexArrayByIndex(index);
    }

    /**
     * update VBO or VEO buffer.
     */
    void updateData(std::vector<float>&& data, uint32_t index = 0, BufferType type = BufferType::VBO) const {
        updateData(std::forward<std::vector<float>&>(data), index, type);
    }
    /**
     * update VBO or VEO buffer.
     */
    void updateData(const std::vector<float>& data, uint32_t index = 0, BufferType type = BufferType::VBO) const {
        if (type == BufferType::VBO) {
            glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObj);
        } else {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vertexEleObj);
        }
        glBufferSubData(type == BufferType::VBO ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER,
            index, data.size() * sizeof(float), data.data());
    }

    void enableVertexArrayByIndex(uint32_t index);
    void disableVertexArrayByIndex(uint32_t index);
    void use() const;
    uint32_t getVAO() const { return _id; }
    uint32_t getVBO() const { return _vertexBufferObj; }
    uint32_t getVEO() const { return _vertexEleObj; }

private:
    uint32_t _id {0};
    uint32_t _vertexEleObj {0};
    uint32_t _vertexBufferObj {0};
    // <vboIndex, enable or disable>
    std::map<uint32_t, bool> _vboIndexMap;
};

NAME_SPACE_END()

#endif // MAIN_VERTEX_ARRAY_OBJ_H
