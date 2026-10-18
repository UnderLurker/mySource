//
// Created by 常笑男 on 2026/10/16.
//
#ifndef _SHADER_COMMON_H
#define _SHADER_COMMON_H

#include <cstdint>
#include <type_traits>
#include "enum.h"

namespace myUtil {

struct VertexArrayInfo {
    uint32_t attrIndex; // 多 buffer 共用同一 VAO 时（每个 buffer 的计数器都从 0 开始）必须显式指定
    int32_t size;
    uint32_t stride;
    uint32_t offset;
    uint32_t divisor = 0;
    bool normalized  = false;
};

template<typename T>
struct DataTypeTraits;

template<>
struct DataTypeTraits<uint8_t> {
    static constexpr DataType type = DataType::UNSIGNED_BYTE;
};
template<>
struct DataTypeTraits<int8_t> {
    static constexpr DataType type = DataType::BYTE;
};
template<>
struct DataTypeTraits<int16_t> {
    static constexpr DataType type = DataType::SHORT;
};
template<>
struct DataTypeTraits<uint16_t> {
    static constexpr DataType type = DataType::UNSIGNED_SHORT;
};
template<>
struct DataTypeTraits<int32_t> {
    static constexpr DataType type = DataType::INT;
};
template<>
struct DataTypeTraits<uint32_t> {
    static constexpr DataType type = DataType::UNSIGNED_INT;
};
template<>
struct DataTypeTraits<float> {
    static constexpr DataType type = DataType::FLOAT;
};

template<typename T, typename = void>
struct IsValidDataType : std::false_type {};

template<typename T>
struct IsValidDataType<T, std::void_t<decltype(DataTypeTraits<T>::type)>> : std::true_type {};

} // namespace myUtil

#endif // !_SHADER_COMMON_H
