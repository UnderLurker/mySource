//
// Created by 常笑男 on 2024/6/15.
//
#include "tree.h"

namespace ULGui::utils {

template<typename T, typename deleter>
void TreeNode<T, deleter>::addNode(node_pointer pointer) {
    CHECK_NULL_VOID(pointer)
    node_pointer temp = _leftChild;
    if (temp == nullptr) {
        _leftChild = pointer;
        return;
    }
    while (temp->_rightChild != nullptr) {
        temp = temp->_rightChild;
    }
    temp->_rightChild = pointer;
}

template<typename T, typename deleter>
template<typename... Args>
void TreeNode<T, deleter>::addNode(Args&&... args) {
    auto* data = new T(args...);
    auto* node = new TreeNode<T, deleter>(data);
    addNode(node);
}
#include <vector>
template<typename T, typename deleter>
void TreeNode<T, deleter>::addBroNode(node_pointer pointer) {
    CHECK_NULL_VOID(pointer)
    node_pointer temp = _leftChild;
    while (temp->_rightChild != nullptr) {
        temp = temp->_rightChild;
    }
    _leftChild = pointer;
}

template<typename T, typename deleter>
template<typename... Args>
void TreeNode<T, deleter>::addBroNode(Args&&... args) {
    auto* data = new T(args...);
    auto* node = new TreeNode<T, deleter>(data);
    addBroNode(node);
}

template<typename T, typename deleter>
size_t TreeNode<T, deleter>::broCount() const {
    node_pointer temp = _leftChild;
    if (temp == nullptr) return 0;
    size_t result = 0;
    while (temp != nullptr) {
        temp = temp->_rightChild;
        result++;
    }
    return result;
}

} // namespace ULGui::utils
