//
// Created by 常笑男 on 2023/10/15.
//

#include <fstream>
#include <sstream>

#include "JsonParse.h"
#include "Reflex.h"
#include "Util.h"

NAME_SPACE_START(myUtil)
namespace {
constexpr char null[] = "null";

bool JsonStrEqual(const JsonStr& left, const JsonStr& right) {
    CHECK_NULL_RETURN(left.getPtr(), false)
    CHECK_NULL_RETURN(right.getPtr(), false)
    if (left.getLength() != right.getLength()) return false;
    for (size_t i = 0; i < left.getLength(); i++) {
        if (*(left.getPtr() + i) != *(right.getPtr() + i)) return false;
    }
    return true;
}

bool JsonNodeEqual(const JsonNode& left, const JsonNode& right) {
    if (left.getKey() != right.getKey() || left.getValue() != right.getValue() || left.getType() != right.getType() ||
        left.getChildren().size() != right.getChildren().size())
        return false;
    if (left.getChildren().empty()) return true;
    if (left.getType() == Array) {
        auto it1 = left.getChildren().begin();
        auto it2 = right.getChildren().begin();
        for (; it1 != left.getChildren().end() && it2 != right.getChildren().end(); it1++, it2++) {
            if (**it1 != **it2) return false;
        }
    } else {
        auto it = left.getChildren().begin();
        for (; it != left.getChildren().end(); it++) {
            if (**it != right[(*it)->getKey()]) {
                return false;
            }
        }
    }
    return true;
}
} // namespace
////////////////////////////////////////////////////
//               JsonDocument                     //
////////////////////////////////////////////////////

JsonDocument::JsonDocument(const char* filePath) { Load(filePath); }

JsonDocument::~JsonDocument() {
    delete[] _cache;
    delete[] _filePath;
    delete _node;
    _cache    = nullptr;
    _filePath = nullptr;
    _node     = nullptr;
    for (auto& item : _memoryPool) {
        delete[] item;
        item = nullptr;
    }
}

JsonStatus JsonDocument::Load(const char* filePath) {
    delete[] _filePath;
    uint32_t length = strlen(filePath);
    _filePath       = new char[length + 1];
    memcpy_s(_filePath, sizeof(char) * length, filePath, length);
    _filePath[length] = '\0';

    fstream file(filePath, ios::in | ios::binary);
    if (file.fail()) return ERROR_FILE_READ;
    file.seekg(0, fstream::end);
    size_t endPos = file.tellg();
    file.seekg(0, fstream::beg);
    _cache  = new char[endPos + 1];
    _length = endPos + 1;
    file.read(_cache, static_cast<long long>(endPos));
    _cache[endPos] = '\0';
    return Analysis();
}

JsonNode& JsonDocument::getNode() const {
    static JsonNode result;
    if (_node == nullptr) return result;
    return *_node;
}

JsonNode* JsonDocument::CreateNode(JsonType type, const char* key, const char* value) {
    auto result = new JsonNode(this, nullptr, type);
    result->setKey(key);
    result->setValue(value);
    return result;
}

void JsonDocument::save(const char* filePath) {
    fstream file(filePath ? filePath : _filePath, ios::out);
    _node->getString(file);
    file.flush();
}

char* JsonDocument::insertMemory(const char* begin, size_t length) {
    auto* tmp = new char[length + 1];
    memcpy_s(tmp, length, begin, length);
    tmp[length] = '\0';
    _memoryPool.emplace_back(tmp);
    return tmp;
}

void JsonDocument::removeMemory(const char* begin) {
    for (auto it = _memoryPool.begin(); it != _memoryPool.end(); it++) {
        if (*it == begin) {
            delete[] *it;
            *it = nullptr;
            _memoryPool.erase(it);
            return;
        }
    }
}

JsonStatus JsonDocument::Analysis() {
    auto tmp = _cache;
    _node    = new JsonNode(tmp, this, nullptr);
    return _node->getStatus();
}

////////////////////////////////////////////////////
//               JsonStringBase                   //
////////////////////////////////////////////////////
void JsonStr::setValue(char* start, size_t length) {
    _ptr    = start;
    _length = length;
}

string JsonStr::getValue() const {
    if (!_ptr) return "";
    return {_ptr, _length};
}

bool JsonStr::operator==(const JsonStr& obj) const { return JsonStrEqual(*this, obj); }

bool JsonStr::operator==(JsonStr&& obj) const { return JsonStrEqual(*this, obj); }

bool JsonStr::operator!=(const JsonStr& obj) const { return !JsonStrEqual(*this, obj); }

bool JsonStr::operator!=(JsonStr&& obj) const { return !JsonStrEqual(*this, obj); }

////////////////////////////////////////////////////
//                    JsonNode                    //
////////////////////////////////////////////////////
JsonNode::JsonNode(const JsonNode& obj) noexcept {
    _key    = obj._key;
    _value  = obj._value;
    _type   = obj._type;
    _status = obj._status;
    _next   = obj._next;
    _parent = obj._parent;
    _doc    = obj._doc;
    for (auto& item : obj._children) {
        _children.emplace_back(item);
    }
}

JsonNode::JsonNode(JsonNode&& obj) noexcept {
    _key      = obj._key;
    _value    = obj._value;
    _type     = obj._type;
    _status   = obj._status;
    _children = obj._children;
    _next     = obj._next;
    _parent   = obj._parent;
    _doc      = obj._doc;
    obj._key.setValue(nullptr, 0);
    obj._value.setValue(nullptr, 0);
    obj._children.clear();
    obj._next   = nullptr;
    obj._parent = nullptr;
    obj._doc    = nullptr;
}

JsonNode::JsonNode(char*& begin, JsonDocument* doc, JsonNode* parent)
    : _doc(doc), _parent(parent) {
    if (begin) _status = Load(begin);
    else _status = ERROR_NULLPTR;
}

JsonNode::~JsonNode() {
    _next   = nullptr;
    _parent = nullptr;
    _doc    = nullptr;
    for_each(_children.begin(), _children.end(), [](JsonNode* node) {
        delete node;
        node = nullptr;
    });
    _children.clear();
}

JsonStatus JsonNode::Load(char*& begin) {
    size_t len = strlen(begin);
    char* end  = begin + len;
    bool isKey = false;
    // 先看:前面的值，可能没有:
    while (begin < end && (*begin == '\r' || *begin == '\n' || *begin == ' '))
        begin++;
    switch (*begin) {
        case '{':
        case '[': {
            _type = *begin == '{' ? Object : Array;
            do {
                auto* childNode = new JsonNode(++begin, _doc, this);
                if (childNode->_value.getPtr() == nullptr && childNode->_key.getPtr() == nullptr &&
                    childNode->_children.empty()) {
                    delete childNode;
                    break;
                }
                _children.emplace_back(childNode);
                runSpace(begin);
            } while (*begin == ',');
            if (*begin == '}' || *begin == ']') {
                begin++;
                return SUCCESS;
            }
            break;
        }
        case '}':
        case ']': {
            begin++;
            return SUCCESS;
        }
        case '"': {
            auto l = begin++;
            while (begin < end && *begin != '"')
                begin++;
            _key.setValue(l + 1, begin - l - 1);
            begin++;
#ifdef _JSON_PARSE_DEBUG
            cout << "key:" << _key.getValue() << endl;
#endif
            isKey = true;
            break;
        }
        default:
            return ERROR_FORMAT;
    }
    return parseValue(begin, end);
}

JsonStatus JsonNode::parseValue(char*& begin, const char* end) {
    CHECK_NULL_RETURN(begin, ERROR_NULLPTR)
    CHECK_NULL_RETURN(end, ERROR_NULLPTR)
    runSpace(begin);
    if (*begin == ':') begin++;
    else if (*begin == ',' || *begin == ']' || *begin == '}') {
        return SUCCESS;
    } else return ERROR_FORMAT;
    runSpace(begin);
    switch (*begin) {
        case '{':
        case '[': {
            _type = *begin == '{' ? Object : Array;
            do {
                auto* childNode = new JsonNode(++begin, _doc, this);
                if (childNode->_value.getPtr() == nullptr && childNode->_key.getPtr() == nullptr &&
                    childNode->_children.empty()) {
                    delete childNode;
                    break;
                }
                _children.emplace_back(childNode);
                runSpace(begin);
            } while (*begin == ',');
            if (*begin == '}' || *begin == ']') {
                begin++;
                return SUCCESS;
            }
            break;
        }
        default: {
            _value = JsonNode::trim(begin);
#ifdef _JSON_PARSE_DEBUG
            cout << "value:" << _value.getValue() << endl;
#endif
            _type = determineType(_value);
            if (_type == None) return ERROR_FORMAT;
            else if (_type == String) {
                _value.setPtr(const_cast<char*>(_value.getPtr()) + 1);
                _value.setLength(_value.getLength() - 2);
            }
        }
    }
    return SUCCESS;
}

JsonStr JsonNode::trim(char*& begin) {
    size_t len = strlen(begin);
    char* end  = begin + len;
    bool quote = false;
    runSpace(begin);
    JsonStr result(begin, 0);
    while (begin < end && *begin != '\n' && (quote || *begin != ',')) {
        if (*begin == '"' && *(begin - 1) != '\\') quote = !quote;
        begin++;
    }
    auto l = begin;
    while (*l == '\n' || *l == '\r' || *l == ' ')
        l--;
    result.setLength(l - result.getPtr() + (*l == ',' ? 0 : 1));
    return result;
}

void JsonNode::runSpace(char*& begin) {
    while (*begin == '\r' || *begin == '\n' || *begin == ' ')
        begin++;
}

JsonType JsonNode::determineType(const JsonStr& input) {
    string str = input.getValue();
    if (str == "true" || str == "false") return Bool;
    else if (legalNumber(str)) return Number;
    else if (str == "null") return Null;
    else if (legalString(str)) return String;
    return None;
}

bool JsonNode::legalNumber(const std::string& str) {
    char* pEnd = nullptr;
    std::strtod(str.c_str(), &pEnd);
    return pEnd != str.c_str();
}

bool JsonNode::legalString(const string& str) {
    if (str.size() < 2) return false;
    if (str[0] == str[str.size() - 1] && str[0] == '\"') return true;
    return false;
}

ostream& JsonNode::getString(ostream& os, uint32_t tab) {
    auto count = static_cast<int32_t>(tab);
    while (count--)
        os << '\t';
    os << (_key.getLength() > 0 ? "\"" : "") << _key.getValue() << (_key.getLength() > 0 ? "\"" : "");
    if (_type == Array || _type == Object) {
        if (_key.getPtr()) os << ": ";
        os << (_type == Array ? "[" : "{");
    } else if (_type == String) os << ": \"" << _value.getValue() << "\"";
    else {
        if (_value.getPtr()) os << ": ";
        os << _value.getValue();
    }
    if (!_children.empty()) {
        os << endl;
        for (const auto& item : _children) {
            item->getString(os, tab + 1);
            if (item != _children.back()) os << ",";
            os << endl;
        }
    }
    if (_type == Array || _type == Object) {
        if (!_children.empty()) {
            count = static_cast<int32_t>(tab);
            while (count--)
                os << '\t';
        }
        os << (_type == Array ? "]" : "}");
    }
    return os;
}

void JsonNode::setKey(const char* begin, size_t length) {
    //    if (_parent == nullptr) return;
    CHECK_NULL_VOID(begin)
    if (length == 0) length = std::strlen(begin);
    if (_memKey) _doc->removeMemory(_key.getPtr());
    _memKey = true;
    _key.setPtr(_doc->insertMemory(begin, length));
    _key.setLength(length);
}

void JsonNode::setValue(const char* begin, size_t length) {
    CHECK_NULL_VOID(begin)
    if (length == 0) length = std::strlen(begin);
    if (_memVal) _doc->removeMemory(_value.getPtr());
    _memVal = true;
    _value.setPtr(_doc->insertMemory(begin, length));
    _value.setLength(length);
}

void JsonNode::setAttr(const char* key, const char* value) {
    CHECK_NULL_VOID(key)
    CHECK_NULL_VOID(value)
    setKey(key);
    setValue(value);
}

JsonNode::JsonAttr JsonNode::getAttr() const { return {getKey(), getValue()}; }

void JsonNode::addChild(JsonNode* node) {
    node->setParent(this);
    _children.emplace_back(node);
}

void JsonNode::addChild(const char* key, const char* value, JsonType type) {
    auto* node = new JsonNode(_doc, this, type);
    node->setKey(key, std::strlen(key));
    node->setValue(value, std::strlen(value));
    _children.emplace_back(node);
}

void JsonNode::removeChild(size_t index) {
    if (index >= _children.size()) return;
    auto it = _children.begin();
    for (uint32_t i = 0; i < _children.size(); i++, it++)
        if (i == index) _children.erase(it);
}

JsonNode& JsonNode::at(size_t index) const {
    auto *result = new JsonNode();
    if (index >= _children.size()) return *result;
    if (_children.size() / 2 > index) {
        auto it = _children.begin();
        for (uint32_t i = 0; i < _children.size(); i++, it++)
            if (i == index) return **it;
    } else {
        auto it = _children.rbegin();
        for (size_t i = _children.size() - 1; i >= 0; i--, it++)
            if (i == index) return **it;
    }
    return *result;
}

JsonNode& JsonNode::at(const string& index) const {
    auto *result = new JsonNode();
    for (auto& item : _children)
        if (strncmp(item->_key.getPtr(), index.c_str(), index.size()) == 0) return *item;
    return *result;
}

JsonNode& JsonNode::operator[](size_t index) const { return at(index); }

JsonNode& JsonNode::operator[](const string& index) const { return at(index); }

bool JsonNode::operator==(const JsonNode& obj) const { return JsonNodeEqual(*this, obj); }

bool JsonNode::operator==(JsonNode&& obj) const { return JsonNodeEqual(*this, obj); }

bool JsonNode::operator!=(const JsonNode& obj) const { return !JsonNodeEqual(*this, obj); }

bool JsonNode::operator!=(JsonNode&& obj) const { return !JsonNodeEqual(*this, obj); }

float JsonNode::toFloat(float defaultValue) const {
    assert(_type == Number);
    char* pEnd  = nullptr;
    auto result = std::strtof(_value.getPtr(), &pEnd);
    if (pEnd == _value.getPtr()) result = defaultValue;
    return result;
}

double JsonNode::toDouble(double defaultValue) const {
    assert(_type == Number);
    char* pEnd  = nullptr;
    auto result = static_cast<double>(std::strtod(_value.getPtr(), &pEnd));
    if (pEnd == _value.getPtr()) result = defaultValue;
    return result;
}

int32_t JsonNode::toInt32(int32_t defaultValue) const {
    assert(_type == Number);
    char* pEnd  = nullptr;
    auto result = static_cast<int32_t>(std::strtol(_value.getPtr(), &pEnd, 10));
    if (pEnd == _value.getPtr()) result = defaultValue;
    return result;
}

int64_t JsonNode::toInt64(int64_t defaultValue) const {
    assert(_type == Number);
    char* pEnd  = nullptr;
    auto result = std::strtoll(_value.getPtr(), &pEnd, 10);
    if (pEnd == _value.getPtr()) result = defaultValue;
    return result;
}

uint32_t JsonNode::toUInt32(uint32_t defaultValue) const {
    assert(_type == Number);
    char* pEnd  = nullptr;
    auto result = static_cast<uint32_t>(std::strtoul(_value.getPtr(), &pEnd, 10));
    if (pEnd == _value.getPtr()) result = defaultValue;
    return result;
}

uint64_t JsonNode::toUInt64(uint64_t defaultValue) const {
    assert(_type == Number);
    char* pEnd  = nullptr;
    auto result = std::strtoull(_value.getPtr(), &pEnd, 10);
    if (pEnd == _value.getPtr()) result = defaultValue;
    return result;
}

bool JsonNode::toBool() const {
    assert(_type == Bool);
    return strncmp(_value.getPtr(), "true", 4) == 0;
}

const char* JsonNode::toNull() const {
    assert(_type == Null);
    return null;
}

string JsonNode::toString() const {
    assert(_type == String);
    return _value.getValue();
}

template<typename T>
void* JsonNode::toRObject(const string& className) {
    assert(_type == Object);
    RObject* result = Singleton<Reflex>::Instance()->createClass(className);
    for (const auto& item : _children) {
        switch (item->_type) {
            case None:
            case String:
                result->set<string>(item->getKey(), item->getValue());
                break;
            case Number: {
                std::string keyType = result->getType(item->getKey());
                size_t keySize      = result->getTypeSize(item->getKey());
                if (keyType == "float") {
                    result->set<float>(item->getKey(), item->toFloat());
                } else if (keyType == "double") {
                    result->set<double>(item->getKey(), item->toDouble());
                } else if ((keyType == "int" || keyType == "int32_t") && keySize == sizeof(int32_t)) {
                    result->set<int32_t>(item->getKey(), item->toInt32());
                } else if (keyType == "uint32_t" && keySize == sizeof(uint32_t)) {
                    result->set<uint32_t>(item->getKey(), item->toUInt32());
                } else if (keyType == "int64_t" && keySize == sizeof(int64_t)) {
                    result->set<int64_t>(item->getKey(), item->toInt64());
                } else if (keyType == "uint64_t" && keySize == sizeof(uint64_t)) {
                    result->set<uint64_t>(item->getKey(), item->toUInt64());
                }
                break;
            }
            case Bool:
                result->set<bool>(item->getKey(), item->toBool());
                break;
            case Null:
                result->set<void*>(item->getKey(), nullptr);
                break;
            default:
                return nullptr;
        }
    }
    return result;
}

template<typename T, typename... Args>
void* JsonNode::toRObject(const string& className, Args... args) {
    assert(_type == Object);
    RObject* result = Singleton<Reflex>::Instance()->createClass(className);
    for (const auto& item : _children) {
        switch (item->_type) {
            case None:
            case String:
                result->set<string>(item->getKey(), item->getValue());
                break;
            case Number: {
                std::string keyType = result->getType(item->getKey());
                size_t keySize      = result->getTypeSize(item->getKey());
                if (keyType == "float") {
                    result->set<float>(item->getKey(), item->toFloat());
                } else if (keyType == "double") {
                    result->set<double>(item->getKey(), item->toDouble());
                } else if ((keyType == "int" || keyType == "int32_t") && keySize == sizeof(int32_t)) {
                    result->set<int32_t>(item->getKey(), item->toInt32());
                } else if (keyType == "uint32_t" && keySize == sizeof(uint32_t)) {
                    result->set<uint32_t>(item->getKey(), item->toUInt32());
                } else if (keyType == "int64_t" && keySize == sizeof(int64_t)) {
                    result->set<int64_t>(item->getKey(), item->toInt64());
                } else if (keyType == "uint64_t" && keySize == sizeof(uint64_t)) {
                    result->set<uint64_t>(item->getKey(), item->toUInt64());
                }
                break;
            }
            case Bool:
                result->set<bool>(item->getKey(), item->toBool());
                break;
            case Null:
                result->set<void*>(item->getKey(), nullptr);
                break;
            case Object:
                result->set<RObject*>(item->getKey(), (RObject*)item->toRObject<Args...>(args...));
                break;
            case Array:
            default:
                return nullptr;
        }
    }
    return result;
}

template<typename T, typename... Args>
void* JsonNode::toRObject(const string& className, const list<string>& subClassName, Args... args) {
    assert(_type == Object);
    RObject* result = Singleton<Reflex>::Instance()->createClass(className);
    auto it         = subClassName.begin();
    for (const auto& item : _children) {
        switch (item->_type) {
            case None:
            case String:
                result->set<string>(item->getKey(), item->getValue());
                break;
            case Number: {
                std::string keyType = result->getType(item->getKey());
                size_t keySize      = result->getTypeSize(item->getKey());
                if (keyType == "float") {
                    result->set<float>(item->getKey(), item->toFloat());
                } else if (keyType == "double") {
                    result->set<double>(item->getKey(), item->toDouble());
                } else if ((keyType == "int" || keyType == "int32_t") && keySize == sizeof(int32_t)) {
                    result->set<int32_t>(item->getKey(), item->toInt32());
                } else if (keyType == "uint32_t" && keySize == sizeof(uint32_t)) {
                    result->set<uint32_t>(item->getKey(), item->toUInt32());
                } else if (keyType == "int64_t" && keySize == sizeof(int64_t)) {
                    result->set<int64_t>(item->getKey(), item->toInt64());
                } else if (keyType == "uint64_t" && keySize == sizeof(uint64_t)) {
                    result->set<uint64_t>(item->getKey(), item->toUInt64());
                }
                break;
            }
            case Bool:
                result->set<bool>(item->getKey(), item->toBool());
                break;
            case Null:
                result->set<void*>(item->getKey(), nullptr);
                break;
            case Object: {
                result->set<RObject*>(item->getKey(), (RObject*)item->toRObject<T, Args...>(*it, args...));
                it++;
                break;
            }
            case Array:
            default:
                return nullptr;
        }
    }
    return result;
}

NAME_SPACE_END()

// myUtil
