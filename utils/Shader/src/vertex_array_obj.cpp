//
// Created by 常笑男 on 2024/3/9.
//
#include "vertex_array_obj.h"

NAME_SPACE_START(myUtil)

void VertexArrayObj::setEleArrayBuffer(const vector<uint32_t>& data, BufferUsage usage) {
    resetVEO();
    glGenBuffers(1, &_vertexEleObj);
    glBindVertexArray(_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vertexEleObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * data.size(), data.data(), usage);
}

void VertexArrayObj::enableVertexArrayByIndex(uint32_t index) {
    if (index >= _vboIndexMap.size() || !_vboIndexMap.count(index)) return;
    glBindVertexArray(_id);
    glEnableVertexAttribArray(index);
    _vboIndexMap[index] = true;
}

void VertexArrayObj::disableVertexArrayByIndex(uint32_t index) {
    if (index >= _vboIndexMap.size() || !_vboIndexMap.count(index)) return;
    glBindVertexArray(_id);
    glDisableVertexAttribArray(index);
    _vboIndexMap[index] = false;
}

void VertexArrayObj::use() const { glBindVertexArray(_id); }

NAME_SPACE_END()
