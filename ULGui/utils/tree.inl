//
// Created by 常笑男 on 2024/6/15.
//
#include "tree.h"

namespace ULGui::utils {

template<typename T, typename deleter>
void TreeNode<T, deleter>::addNode(node_pointer pointer) {
    CHECK_NULL_VOID(pointer)
    pointer->_parent = this;
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

template<typename T, typename deleter>
void TreeNode<T, deleter>::addBroNode(node_pointer pointer) {
    CHECK_NULL_VOID(pointer)
    pointer->_parent = _parent;
    node_pointer temp = this;
    while (temp->_rightChild != nullptr) {
        temp = temp->_rightChild;
    }
    temp->_rightChild = pointer;
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

template<typename T, typename deleter>
size_t TreeNode<T, deleter>::depth() const {
    if (_leftChild == nullptr) return 0;
    size_t result = 0;
    node_pointer temp = _leftChild;
    while(temp != nullptr) {
        result = std::max(result, temp->depth() + 1);
        temp = temp->_rightChild;
    }
    return result;
}

} // namespace ULGui::utils
