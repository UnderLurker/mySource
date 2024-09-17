#include "BTree.h"

#include <cmath>

namespace myUtil {
constexpr float HALF = 0.5f;

template<typename V, size_t N, typename Compare>
BTreeNode<V, N, Compare>::BTreeNode(const BTreeNode& obj) {
    _value             = new ValueType(*(obj._value));
    _selfElement       = obj._selfElement;
    _leftElementChild  = obj._leftElementChild;
    _rightElementChild = obj._rightElementChild;
}

template<typename V, size_t N, typename Compare>
BTreeNode<V, N, Compare>::BTreeNode(BTreeNode&& obj) noexcept {
    _value                 = obj._value;
    _selfElement           = obj._selfElement;
    _leftElementChild      = obj._leftElementChild;
    _rightElementChild     = obj._rightElementChild;
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

template<typename V, size_t N, typename Compare>
BTreeNode<V, N, Compare>& BTreeNode<V, N, Compare>::operator=(const BTreeNode& obj) {
    _value             = new ValueType(*(obj._value));
    _selfElement       = obj._selfElement;
    _leftElementChild  = obj._leftElementChild;
    _rightElementChild = obj._rightElementChild;
    return *this;
}
/////////////////////////////////////////////////////////////////
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
void BTreeElement<V, N, Compare>::checkOverflow(const BTreePos<V, N, Compare>& position) {
    size_t length = _valueList.size();
    if (length > N) {
        updateUpOverflow(position);
    } else if (length < std::ceil(N * HALF)) {
        updateDownOverflow(position);
    }
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::updateUpOverflow(const BTreePos<V, N, Compare>& position) {
    size_t halfSize = std::ceil(N * HALF);
    size_t midPos = std::floor(N * HALF);
    if (_valueList.size() > N) {

    }
}

template<typename V, size_t N, typename Compare>
void BTreeElement<V, N, Compare>::updateDownOverflow(const BTreePos<V, N, Compare>& position) {
    size_t halfSize = std::ceil(N * HALF);
    if (_leftNodeParent && _leftNodeParent->_leftElementChild &&
        _leftNodeParent->_leftElementChild->size() > halfSize) {
        auto leftBrother = _leftNodeParent->_leftElementChild;
        auto leftNode    = leftBrother->_valueList.back();
        auto newNode     = new BTreeNode<V, N, Compare>(_leftNodeParent->_value, leftNode->_rightElementChild,
                                                    _valueList.front()->_leftElementChild, this);
        _valueList.insert(_valueList.begin(), newNode);
        _leftNodeParent->_value = leftNode->_value;
        leftBrother->_valueList.pop_back();
    } else if (_rightNodeParent && _rightNodeParent->_rightElementChild &&
               _rightNodeParent->_rightElementChild->size() > halfSize) {
        auto rightBrother = _rightNodeParent->_rightElementChild;
        auto rightNode    = rightBrother->_valueList.front();
        auto newNode = new BTreeNode<V, N, Compare>(_rightNodeParent->_value, _valueList.back()->_rightElementChild,
                                                    rightNode->_leftElementChild, this);
        _valueList.insert(_valueList.rbegin(), newNode);
        _rightNodeParent->_value = rightNode->_value;
        rightNode->_valueList.pop_front();
    } else {
    }
}

/////////////////////////////////////////////////////////////////
} // namespace myUtil
