//
// Created by 常笑男 on 2026/10/16.
//

#ifndef _VERTEX_BUFFER_OBJ_H
#define _VERTEX_BUFFER_OBJ_H

#include <cstddef>
#include <glad/glad.h>
#include <vector>

#include "common.h"

namespace myUtil {

class VertexArrayObj;

class Buffer {
public:
    Buffer(BufferType type)
        : _type(type) {
        glGenBuffers(1, &_id);
    }
    virtual ~Buffer() {
        if (_id != 0) glDeleteBuffers(1, &_id);
    }

    /**
     * @brief for VBO, VEO
     */
    void bind() const {
        if (_type == BufferType::UBO) return;
        glBindBuffer(Target(), _id);
    }

    /**
     * @brief for UBO
     * @param binding glsl binding index
     */
    void bind(uint32_t binding) const {
        if (_type != BufferType::UBO) return;
        glBindBufferBase(Target(), binding, _id);
    }

    /**
     * @brief for UBO
     * @param binding glsl binding index
     * @param offset buffer data offset
     * @param size buffer data size
     */
    void bind(uint32_t binding, uint64_t offset, uint64_t size) const {
        if (_type != BufferType::UBO) return;
        glBindBufferRange(Target(), binding, _id, offset, size);
    }

    /**
     * @brief for VBO, VEO
     */
    void unBind() const {
        if (_type == BufferType::UBO) return;
        glBindBuffer(Target(), 0);
    }

    /**
     * @brief for UBO
     * @param binding glsl binding index
     */
    void unBind(uint32_t binding) const {
        if (_type != BufferType::UBO) return;
        glBindBufferBase(Target(), binding, 0);
    }

    /**
     * @brief for UBO
     * @param binding glsl binding index
     */
    void unBind(uint32_t binding, uint64_t, uint64_t) const {
        if (_type != BufferType::UBO) return;
        glBindBufferRange(Target(), binding, 0, 0, 0);
    }
    uint32_t id() const { return _id; }
    BufferType type() const { return _type; }

    template<typename T>
    void setData(const std::vector<T>& data, BufferUsage usage) {
        setBufferData((void*)data.data(), data.size(), usage);
    }

    void setData(int64_t size, BufferUsage usage) { setBufferData(size, usage); }

    template<typename T>
    void update(const std::vector<T>& data, int64_t offset = 0) {
        updateBufferData((void*)data.data(), data.size(), offset);
    }

    bool operator==(const Buffer& o) { return _id == o._id && _type == o._type; }

    bool operator!=(const Buffer& o) { return !(*this == o); }

protected:
    virtual void setBufferData(void*, int64_t, BufferUsage) {}

    virtual void setBufferData(int64_t, BufferUsage) {}

    virtual void updateBufferData(void*, int64_t, int64_t = 0) {}

    virtual void setVertexAttr(uint32_t, int32_t, uint32_t, uint32_t, uint32_t = 0, bool = false) {}

    GLenum Target() const {
        switch (_type) {
            case BufferType::VBO:
                return GL_ARRAY_BUFFER;
            case BufferType::VEO:
                return GL_ELEMENT_ARRAY_BUFFER;
            case BufferType::UBO:
                return GL_UNIFORM_BUFFER;
        }
        return GL_ARRAY_BUFFER;
    }

    Buffer(const Buffer&)            = delete;
    Buffer(Buffer&&)                 = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&)      = delete;

protected:
    uint32_t _id {0};
    BufferType _type;
    friend VertexArrayObj;
};

template<typename T>
class BufferT : public Buffer {
    static_assert(IsValidDataType<T>::value,
                  "BufferT<T>: T must be one of int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t/float");

public:
    BufferT(BufferType type)
        : Buffer(type) {}

private:
    void setBufferData(void* data, int64_t size, BufferUsage usage) override {
        bind();
        glBufferData(Target(), sizeof(T) * size, data, usage);
    }

    void setBufferData(int64_t size, BufferUsage usage) override {
        bind();
        glBufferData(Target(), sizeof(T) * size, nullptr, usage);
    }

    void updateBufferData(void* data, int64_t size, int64_t offset = 0) override {
        bind();
        glBufferSubData(Target(), offset * sizeof(T), sizeof(T) * size, data);
    }

    void setVertexAttr(uint32_t attrIndex,
                       int32_t size,
                       uint32_t stride,
                       uint32_t offset,
                       uint32_t divisor = 0,
                       bool normalized  = false) override {
        bind();
        glEnableVertexAttribArray(attrIndex);
        glVertexAttribPointer(attrIndex, size, DataTypeTraits<T>::type, normalized ? GL_TRUE : GL_FALSE,
                              sizeof(T) * stride, (void*)(sizeof(T) * offset));
        glVertexAttribDivisor(attrIndex, divisor);
    }
};

using BufferF   = BufferT<float>;
using BufferI8  = BufferT<int8_t>;
using BufferU8  = BufferT<uint8_t>;
using BufferI16 = BufferT<int16_t>;
using BufferU16 = BufferT<uint16_t>;
using BufferI32 = BufferT<int32_t>;
using BufferU32 = BufferT<uint32_t>;
} // namespace myUtil

#endif // !_VERTEX_BUFFER_OBJ_H
