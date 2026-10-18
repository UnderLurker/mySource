//
// Created by 常笑男 on 2026/10/16.
//

#ifndef _VERTEX_ENUM_H
#define _VERTEX_ENUM_H

namespace myUtil {

enum DataType {
    BYTE           = 0x1400,
    UNSIGNED_BYTE  = 0x1401,
    SHORT          = 0x1402,
    UNSIGNED_SHORT = 0x1403,
    INT            = 0x1404,
    UNSIGNED_INT   = 0x1405,
    FLOAT          = 0x1406
};

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
    VBO, // 前置需要VAO
    VEO, // 前置需要VAO
    UBO, // 可单独使用
};

} // namespace myUtil

#endif // !_VERTEX_ENUM_H
