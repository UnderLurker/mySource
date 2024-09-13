//
// Created by 常笑男 on 2024/9/7.
//

#ifndef _TRIE_H
#define _TRIE_H

#include "Tree.h"

namespace myUtil {

template<typename T, typename Compare = std::less<T>>
class Trie : public Tree<T, Compare> {
private:
};

} // namespace myUtil
#endif // _TRIE_H
