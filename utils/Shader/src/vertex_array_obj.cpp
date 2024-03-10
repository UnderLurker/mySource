//
// Created by 常笑男 on 2024/3/9.
//
#include "vertex_array_obj.h"

NAME_SPACE_START(myUtil)

void VertexArrayObj::setEleArrayBuffer(const vector<uint32_t>& data, BufferUsage usage) {
    glGenBuffers(1, &_vertexEleObj);
    glBindVertexArray(_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vertexEleObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * data.size(), data.data(), usage);
}

void VertexArrayObj::enableVertexArrayByIndex(uint32_t index) const {
    glBindVertexArray(_id);
    for (const auto& item : _vboIndexList)
        if (item == index) {
            glEnableVertexAttribArray(index);
            break;
        }
}

void VertexArrayObj::disableVertexArrayByIndex(uint32_t index) const {
    glBindVertexArray(_id);
    for (const auto& item : _vboIndexList)
        if (item == index) {
            glDisableVertexAttribArray(index);
            break;
        }
}

void VertexArrayObj::use() const { glBindVertexArray(_id); }

void VertexArrayObj::deleteVAO() {
    if (_id != 0) glDeleteVertexArrays(1, &_id);
    if (_vertexEleObj != 0) glDeleteBuffers(1, &_vertexEleObj);
    if (_vertexBufferObj != 0) glDeleteBuffers(1, &_vertexBufferObj);
}

NAME_SPACE_END()
