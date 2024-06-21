//
// Created by XW on 2024/6/12.
//

#ifndef _TREE_H
#define _TREE_H

#include <functional>
#include <map>

#include "..\include\ULGuiMacro.h"

namespace ULGui::utils {

template<typename T>
struct TreeDeleter {
    void operator()(T** pointer) {
        CHECK_NULL_VOID(pointer)
        CHECK_NULL_VOID(*pointer)
        delete *pointer;
        *pointer = nullptr;
    }
};

//      tree struct
//      ┌──────────────┐
//      │     root     │
//      └──────────────┘
//         /             \
// ┌────────────┐    ┌──────────┐
// │root's child│    │root's bro│
// └────────────┘    └──────────┘
//
template<typename T, typename deleter = TreeDeleter<T>>
class TreeNode {
    using value_type         = T;
    using value_reference    = T&;
    using value_pointer      = T*;
    using node_pointer       = TreeNode<value_type, deleter>*;
    using const_node_pointer = const TreeNode<value_type, deleter>*;

public:
    TreeNode() = default;
    explicit TreeNode(std::nullptr_t);
    explicit TreeNode(node_pointer pointer)
        : _parent(pointer) {}
    explicit TreeNode(const value_type& value, node_pointer parent)
        : _data(value), _parent(parent) {}
    template<typename... Args>
    explicit TreeNode(Args&&... args)
        : _data(new T(args...)) {}
    virtual ~TreeNode() {
        deleter {}(&_data);
        _parent = nullptr;
        delete _leftChild;
        delete _rightChild;
    }
    /**
     * get parent node
     * @return
     */
    node_pointer parent() { return _parent; }
    /**
     * set parent node
     * @param pointer
     */
    void setParent(node_pointer pointer) { CHECK_NULL_VOID(pointer) _parent = pointer; }
    /**
     * add TreeNode<T, deleter>
     * @param pointer
     */
    void addNode(node_pointer pointer);
    /**
     * add TreeNode<T, deleter>
     * @tparam Args value_type constructor
     * @param args value_type
     * constructor
     */
    template<typename... Args>
    void addNode(Args&&... args);
    /**
     * add brother node
     * @param pointer
     */
    void addBroNode(node_pointer pointer);
    /**
     * add brother node
     * @tparam Args
     * @param args
     */
    template<typename... Args>
    void addBroNode(Args&&... args);
    /**
     * get brother node count
     * @return
     */
    size_t broCount() const;
    // if depth large more than stack, crash
    // not implement at the moment
    size_t depth() const;

private:
    node_pointer _parent {nullptr};
    node_pointer _leftChild {nullptr};
    node_pointer _rightChild {nullptr};
    value_pointer _data {nullptr};
};

template<typename T, typename deleter = TreeDeleter<T>>
class Tree {
    using node           = TreeNode<T, deleter>;
    using node_reference = node&;
    using node_pointer   = node*;

public:
    Tree()          = default;
    virtual ~Tree() = default;

    node_reference root() { return _root; }
    node_reference root() const { return _root; }
    size_t depth() const { return _root.depth(); }

private:
    TreeNode<T, deleter> _root;
};

} // namespace ULGui::utils
#include "tree.inl"
#endif // _TREE_H
