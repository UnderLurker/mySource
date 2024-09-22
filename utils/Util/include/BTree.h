//
// Created by 常笑男 on 2024/8/18.
//

#ifndef _BTREE_H
#define _BTREE_H

#include <cassert>
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
    BTreeNode(const BTreeNode& obj) = delete;
    BTreeNode(BTreeNode&& obj) noexcept;
    explicit BTreeNode(const ValueType& obj,
                       ElementType* leftElementChild  = nullptr,
                       ElementType* rightElementChild = nullptr,
                       ElementType* selfElement       = nullptr)
        : _value(new ValueType(obj)),
          _leftElementChild(leftElementChild),
          _rightElementChild(rightElementChild),
          _selfElement(selfElement) {}
    explicit BTreeNode(ValueType* obj,
                       ElementType* leftElementChild  = nullptr,
                       ElementType* rightElementChild = nullptr,
                       ElementType* selfElement       = nullptr)
        : _value(new ValueType(obj)),
          _leftElementChild(leftElementChild),
          _rightElementChild(rightElementChild),
          _selfElement(selfElement) {}
    virtual ~BTreeNode();
    BTreeNode& operator=(const BTreeNode& obj) = delete;

    const ValueType& value() const { return *_value; }
    ValueType& value() { return *_value; }
    const ElementType& selfElement() const { *_selfElement; }
    const ElementType& leftElement() const { *_leftElementChild; }
    const ElementType& rightElement() const { *_rightElementChild; }
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
    BTreeElement() = default;
    BTreeElement(const ElementType& obj) = delete;
    ElementType& operator=(const ElementType&) = delete;
    BTreeElement(ElementType&& obj) noexcept;
    /**
     *
     * @param nodeList
     * @param leftParent
     * @param rightParent
     * @param pos 元素所在父元素的list中的位置，用来快速定位
     * @param parent
     */
    explicit BTreeElement(const std::list<NodeType*>& nodeList,
                          NodeType* leftParent  = nullptr,
                          NodeType* rightParent = nullptr,
                          size_t pos            = 0,
                          ElementType* parent   = nullptr)
        : _valueList(nodeList),
          _leftNodeParent(leftParent),
          _rightNodeParent(rightParent),
          _pos(pos),
          _parentElement(parent) { assert(pos >= 0); }
    virtual ~BTreeElement();

    size_t size() const { return _valueList.size(); }
    template<typename... Args>
    void emplace(Args&&... args) { emplace({ValueType(args...)}); }
    void emplace(const ValueType& value) { emplace({value}); }
    void emplace(const NodeType& value);
    void emplace(const ElementType& element);
    Position find(const ValueType& value);
    void erase(const Position& position);
    NodeType*& front() { return _valueList.front(); }
    NodeType*& back() { return _valueList.back(); }
    bool empty() const { return _valueList.empty(); }

protected:
    void checkOverflow();
    void updateUpOverflow();
    void updateDownOverflow();
    void borrowLeft();
    void borrowRight();
    void mergeLeft();
    void mergeRight();

private:
    std::list<NodeType*> _valueList;
    NodeType* _leftNodeParent {nullptr};
    NodeType* _rightNodeParent {nullptr};
    ElementType* _parentElement {nullptr};
    size_t _pos {0};

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
public:
private:
};

} // namespace myUtil

#include "BTree.inl"
#endif // _BTREE_H
