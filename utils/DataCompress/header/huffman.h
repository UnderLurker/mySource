//
// Created by 常笑男 on 2024/2/11.
//

#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include "Util.h"
#include "heap.h"

NAME_SPACE_START(myUtil)

template<typename T>
class huffman {
public:
private:
    heap<T> _smallHeap;
    unordered_map<string, T> _binaryMap;
    uint8_t *result { nullptr };
};

NAME_SPACE_END()

#include "huffman.inl"

#endif // !_HUFFMAN_H
