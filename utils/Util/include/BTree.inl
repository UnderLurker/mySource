#include "BTree.h"

#include <cmath>

namespace myUtil {
namespace {
constexpr float HALF = 0.5f;
}

//template<typename V, size_t N, typename Compare>
//BTreeNode<V, N, Compare>::BTreeNode(const BTreeNode& obj) {
//    _value             = new ValueType(*(obj._value));
//    _selfElement       = obj._selfElement;
//    _leftElementChild  = obj._leftElementChild;
//    _rightElementChild = obj._rightElementChild;
//}

template<typename V, size_t N, typename Compare>
BTreeNode<V, N, Compare>::BTreeNode(BTreeNode&& obj) noexcept
    : _value(obj._value),
      _selfElement(obj._selfElement),
      _leftElementChild(obj._leftElementChild),
      _rightElementChild(obj._rightElementChild) {
    obj._value             = nullptr;
    obj._selfElement       = nullptr;
    obj._leftElementChild  = nullptr;
    obj._rightElementChild = nullptr;
}

template<typename V, size_t N, typename Compare>
BTreeNode<V, N, Compare>::~BTreeNode() {
    delete _value;
    _value             = nullptr;
    _leftElementChild  = nullptr;
    _rightElementChild = nullptr;
}

//template<typename V, size_t N, typename Compare>
//BTreeNode<V, N, Compare>& BTreeNode<V, N, Compare>::operator=(const BTreeNode& obj) {
//    _value             = new ValueType(*(obj._value));
//    _selfElement       = obj._selfElement;
//    _leftElementChild  = obj._leftElementChild;
//    _rightElementChild = obj._rightElementChild;
//    return *this;
//}
/////////////////////////////////////////////////////////////////
template<typename V, size_t N, typename Compare>
BTreeElement<V, N, Compare>::BTreeElement(ElementType&& obj) noexcept
    : _valueList(std::move(obj._valueList)),
      _leftNodeParent(obj._leftNodeParent),
      _rightNodeParent(obj._rightNodeParent),
      _parentElement(obj._parentElement) {
    obj._leftNodeParent  = nullptr;
    obj._rightNodeParent = nullptr;
    obj._parentElement   = nullptr;
}

template<typename V, size_t N, typename Compare>
BTreeElement<V, N, Compare>::~BTreeElement() {
    for (auto& item : _valueList) {
        delete item;
        item = nullptr;
    }
    _valueList.clear();
    _leftNodeParent  = nullptr;
    _rightNodeParent = nullptr;
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::emplace(const NodeType& value) {

}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::emplace(const ElementType& value) {

}

template<typename V, size_t N, typename Compare>
BTreePos<V, N, Compare> BTreeElement<V, N, Compare>::find(const ValueType& value) {
    size_t pos = 0;
    for (auto&& item : _valueList) {
        if (value == item->value()) {
            return {item, pos};
        } else if (Compare(value, item->value())) {
            return item->_leftElementChild->find(value);
        }
        pos++;
    }
    if (!_valueList.empty()) {
        return _valueList.back()->_rightElementChild->find(value);
    }
    return {nullptr, 0};
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::erase(const BTreePos<V, N, Compare>& position) {

}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::checkOverflow() {
    size_t length = _valueList.size();
    if (length > N) {
        updateUpOverflow();
    } else if (length < static_cast<size_t>(std::ceil(N * HALF))) {
        updateDownOverflow();
    }
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::updateUpOverflow() {
    size_t midPos   = std::floor(N * HALF);
    if (_valueList.size() <= N) return;
    auto newNode = *(_valueList.begin() + midPos);
    _valueList.erase(_valueList.begin() + midPos);
    auto leftElement  = this;
    auto rightElement = new ElementType({_valueList.begin() + midPos, _valueList.end()}, newNode, _rightNodeParent,
                                        _pos + 1, _parentElement);
    _valueList.erase(_valueList.begin() + midPos, _valueList.end());
    _rightNodeParent = newNode;
    for (auto&& item : rightElement->_valueList) {
        item->_selfElement = rightElement;
    }
    newNode->_leftElementChild  = leftElement;
    newNode->_rightElementChild = rightElement;
    if (_parentElement) {
        newNode->_selfElement = _parentElement;
        // 此时为中间节点或者叶子节点
        _parentElement->_valueList.insert(_parentElement->_valueList.begin() + _pos, newNode);
        if (_pos < _parentElement->size()) {
            auto it               = *(_parentElement->_valueList.begin() + rightElement->_pos);
            it->_leftElementChild = rightElement;
        }
        _parentElement->updateUpOverflow();
        return;
    }
    // 此时已经为根节点，需要创建parentElement
    _parentElement        = new ElementType({newNode});
    newNode->_selfElement = _parentElement;
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::updateDownOverflow() {
    size_t halfSize = std::ceil(N * HALF);
    if (_leftNodeParent && _leftNodeParent->_leftElementChild &&
        _leftNodeParent->_leftElementChild->size() > halfSize) {
        mergeLeft();
    } else if (_rightNodeParent && _rightNodeParent->_rightElementChild &&
               _rightNodeParent->_rightElementChild->size() > halfSize) {
        mergeRight();
    } else if (_leftNodeParent && _leftNodeParent->_leftElementChild) {
        borrowLeft();
    } else if (_rightNodeParent && _rightNodeParent->_rightElementChild) {
        borrowRight();
    }
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::borrowLeft() {
    auto leftBrother = _leftNodeParent->_leftElementChild;
    auto leftNode    = front()->_leftElementChild;
    auto newNode =
        new NodeType(_leftNodeParent->_value, leftBrother->back()->_rightElementChild, leftNode, leftBrother);
    leftNode->_parentElement  = leftBrother;
    leftNode->_leftNodeParent = newNode;
    leftNode->_pos            = leftNode->size();
    leftBrother->_valueList.emplace(newNode);
    _parentElement->_valueList.erase(_parentElement->_valueList.begin() + _pos);
    size_t num = leftBrother->size();
    for (auto&& item : _valueList) {
        item->_rightElementChild->_pos           += num;
        item->_rightElementChild->_parentElement  = leftBrother;
    }
    leftBrother->_valueList.merge(_valueList);
    _valueList.clear();
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::borrowRight() {
    auto rightBrother = _rightNodeParent->_rightElementChild;
    auto rightNode    = back()->_rightElementChild;
    auto newNode =
        new NodeType(_rightNodeParent->_value, rightNode, rightBrother->front()->_leftElementChild, rightBrother);
    rightNode->_parentElement   = rightBrother;
    rightNode->_rightNodeParent = newNode;
    rightBrother->_valueList.insert(rightBrother->_valueList.begin(), newNode);
    for (auto&& item : rightBrother->_valueList) {
        item->_rightElementChild->_pos += _valueList.size() + 1;
    }
    for (auto&& item : _valueList) {
        item->_leftElementChild->_parentElement = rightBrother;
    }
    rightBrother->_valueList.insert(rightBrother->_valueList.begin(), _valueList);
    _valueList.clear();
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::mergeLeft() {
    auto leftBrother = _leftNodeParent->_leftElementChild;
    auto leftNode    = leftBrother->back();
    auto newNode     = new NodeType(_leftNodeParent->_value, leftNode->_rightElementChild,
                                    _valueList.front()->_leftElementChild, this);
    _valueList.insert(_valueList.begin(), newNode);
    _leftNodeParent->_value = leftNode->_value;
    leftBrother->_valueList.pop_back();
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::mergeRight() {
    auto rightBrother = _rightNodeParent->_rightElementChild;
    auto rightNode    = rightBrother->front();
    auto newNode      = new NodeType(_rightNodeParent->_value, _valueList.back()->_rightElementChild,
                                     rightNode->_leftElementChild, this);
    _valueList.insert(_valueList.rbegin(), newNode);
    _rightNodeParent->_value = rightNode->_value;
    rightNode->_valueList.pop_front();
}

/////////////////////////////////////////////////////////////////
} // namespace myUtil
