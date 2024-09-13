//
// Created by 常笑男 on 2024/8/18.
//

#ifndef _BTREE_H
#define _BTREE_H

#include <functional>
#include <vector>

namespace myUtil {

template<typename V, typename Compare = std::less<V>>
class BTreeNode {
    using valueType = V;
public:
    BTreeNode() = delete;
    explicit BTreeNode(uint32_t length) {

    }

private:
    std::vector<valueType*> _data;
    std::vector<BTreeNode*> _children;
};


class BTree {

};

}
#endif // _BTREE_H
