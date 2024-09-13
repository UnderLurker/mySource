//
// Created by 常笑男 on 2024/9/7.
//

#ifndef _TREE_H
#define _TREE_H

#include <deque>
#include <memory>
#include <optional>

namespace myUtil {

template<typename value_type, typename Compare = std::less<value_type>>
class Tree {
public:
    template<typename Type>
    struct TreeNode {
    private:
        using NodeType  = TreeNode<Type>;
        using DequeType = std::deque<NodeType>;

    public:
        std::optional<Type> value;
        std::shared_ptr<DequeType> children;
    };

private:
    using T        = value_type;
    using NodeType = TreeNode<T>;

public:
    Tree()          = default;
    virtual ~Tree() = default;
    void clear();
    void addNode(const NodeType& node);

private:
    NodeType* _root {nullptr};
};

} // namespace myUtil
#endif // _TREE_H
