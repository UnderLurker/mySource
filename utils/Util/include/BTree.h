//
// Created by 常笑男 on 2024/8/18.
//

#ifndef _BTREE_H
#define _BTREE_H

#include <functional>
#include <list>
#include <memory>
#include <vector>

namespace myUtil {

template<typename V, size_t N, typename Compare>
class BTreeElement;

/**
 * BTree element node
 * @tparam V Value type
 * @tparam N Maximum number of nodes
 * @tparam Compare compare function
 */
template<typename V, size_t N, typename Compare = std::less<V>>
class BTreeNode {
    using ValueType   = V;
    using ElementType = BTreeElement<ValueType, N, Compare>;

public:
    BTreeNode() = delete;
    BTreeNode(const BTreeNode& obj);
    BTreeNode(BTreeNode&& obj) noexcept;
    explicit BTreeNode(const ValueType& obj,
                       ElementType* leftElementChild  = nullptr,
                       ElementType* rightElementChild = nullptr)
        : _value(new ValueType(obj)), _leftElementChild(leftElementChild), _rightElementChild(rightElementChild) {}
    explicit BTreeNode(ValueType* obj,
                       ElementType* leftElementChild  = nullptr,
                       ElementType* rightElementChild = nullptr)
        : _value(new ValueType(obj)), _leftElementChild(leftElementChild), _rightElementChild(rightElementChild) {}
    virtual ~BTreeNode();
    BTreeNode& operator=(const BTreeNode& obj);

    const ValueType& value() const { return *_value; }
    ValueType& value() { return *_value; }
private:
    ValueType* _value {nullptr};
    ElementType* _selfElement {nullptr};
    ElementType* _leftElementChild {nullptr};
    ElementType* _rightElementChild {nullptr};
    friend BTreeElement<V, N, Compare>;
};

template<typename V, size_t N, typename Compare = std::less<V>>
using BTreePos = std::pair<BTreeNode<V, N, Compare>*, size_t>;
/**
 * BTree element node, include BTreeNodes
 * @tparam V Value type
 * @tparam N Maximum number of nodes
 * @tparam Compare compare function
 */
template<typename V, size_t N, typename Compare = std::less<V>>
class BTreeElement {
    using ValueType = V;
    using NodeType  = BTreeNode<ValueType, N, Compare>;
    using ElementType = BTreeElement<ValueType, N, Compare>;
    using Position = BTreePos<V, N, Compare>;
public:
public:
    virtual ~BTreeElement();

    size_t size() const { return _valueList.size(); }
    template<typename... Args>
    void emplace(Args&&... args) { emplace({ValueType(args...)}); }
    void emplace(const ValueType& value) { emplace({value}); }
    void emplace(const NodeType& value);
    void emplace(const ElementType& element);
    Position find(const ValueType& value);
    void erase(const Position& position);

protected:
    void checkOverflow(const Position& position);
    void updateUpOverflow(const Position& position);
    void updateDownOverflow(const Position& position);

private:
    std::list<NodeType*> _valueList;
    NodeType* _leftNodeParent {nullptr};
    NodeType* _rightNodeParent {nullptr};

};

/**
 * BTree
 * @tparam V Value type
 * @tparam N Maximum number of nodes
 * @tparam Compare compare function
 */
template<typename V, size_t N, typename Compare = std::less<V>>
class BTree {
    using ValueType   = V;
    using NodeType    = BTreeNode<ValueType, N, Compare>;
    using ElementType = BTreeElement<ValueType, N, Compare>;
};

} // namespace myUtil

#include "BTree.inl"
#endif // _BTREE_H
