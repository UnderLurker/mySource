//
// Created by 常笑男 on 2024/3/9.
//

#ifndef MAIN_VERTEX_ARRAY_OBJ_H
#define MAIN_VERTEX_ARRAY_OBJ_H

#include <glad/glad.h>

#include "Util.h"

NAME_SPACE_START(myUtil)

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
    VertexArrayObj() { glGenVertexArrays(1, &_id); }
    virtual ~VertexArrayObj() = default;

    template<typename T>
    void setArrayBuffer(const vector<T>& data, BufferUsage usage) {
        glGenBuffers(1, &_vertexBufferObj);
        glBindVertexArray(_id);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObj);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), usage);
    }

    void setEleArrayBuffer(const vector<uint32_t>& data, BufferUsage usage);

    template<typename T>
    void
        setVertexAttribPointer(int32_t size, DataType type, uint32_t stride, uint32_t offset, bool normalized = false) {
        glBindVertexArray(_id);
        uint32_t index = _vboIndexList.size();
        glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, sizeof(T) * stride,
                              (void*)(sizeof(T) * offset));
        _vboIndexList.push_back(index);
        enableVertexArrayByIndex(index);
    }

    void enableVertexArrayByIndex(uint32_t index) const;
    void disableVertexArrayByIndex(uint32_t index) const;
    void use() const;
    void deleteVAO();

private:
    uint32_t _id {0};
    uint32_t _vertexEleObj {0};
    uint32_t _vertexBufferObj {0};
    vector<uint32_t> _vboIndexList;
};

NAME_SPACE_END()

#endif // MAIN_VERTEX_ARRAY_OBJ_H
