//
// Created by 常笑男 on 2023/10/15.
//

#include "JsonParse.h"

#include <fstream>
#include <sstream>

#include "Util.h"

NAME_SPACE_START(myUtil)

////////////////////////////////////////////////////
//               JsonDocument                     //
////////////////////////////////////////////////////

JsonDocument::JsonDocument(const char *filePath) { Load(filePath); }

JsonDocument::~JsonDocument() {
    delete[] _cache;
    delete[] _filePath;
    _cache    = nullptr;
    _filePath = nullptr;
}

JsonStatus JsonDocument::Load(const char *filePath) {
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
    file.read(_cache, endPos);
    _cache[endPos] = '\0';
    return Analysis();
}

void JsonDocument::save(const char *filePath) {
    fstream file(filePath ? filePath : _filePath, ios::out);
    _node->getString(file);
    file.flush();
}

JsonStatus JsonDocument::Analysis() {
    auto tmp = _cache;
    _node    = new JsonNode(tmp, this, nullptr);
    return _node->getStatus();
}

////////////////////////////////////////////////////
//               JsonStringBase                   //
////////////////////////////////////////////////////
void JsonStr::setValue(char *start, size_t length) {
    _ptr    = start;
    _length = length;
}

string JsonStr::getValue() const {
    if (!_ptr) return "";
    return string(_ptr, _length);
}

////////////////////////////////////////////////////
JsonNode::JsonNode(JsonNode &&obj) noexcept {
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

JsonNode::JsonNode(char *&begin, JsonDocument *doc, JsonNode *parent)
    : _doc(doc), _parent(parent) {
    _status = Load(begin);
}

JsonStatus JsonNode::Load(char *&begin) {
    size_t len = strlen(begin);
    char *end  = begin + len;
    bool isKey = false;
    // 先看:前面的值，可能没有:
    while (begin < end && (*begin == '\r' || *begin == '\n' || *begin == ' '))
        begin++;
    switch (*begin) {
        case '{':
        case '[': {
            _type = *begin == '{' ? Object : Array;
            do {
                auto *childNode = new JsonNode(++begin, _doc, this);
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
            _key.setValue(l, begin - l + 1);
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

JsonStatus JsonNode::parseValue(char *&begin, const char *end) {
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
                auto *childNode = new JsonNode(++begin, _doc, this);
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
        }
    }
    return SUCCESS;
}

JsonStr JsonNode::trim(char *&begin) {
    size_t len = strlen(begin);
    char *end  = begin + len;
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

void JsonNode::runSpace(char *&begin) {
    while (*begin == '\r' || *begin == '\n' || *begin == ' ')
        begin++;
}

JsonType JsonNode::determineType(const JsonStr &input) {
    string str = input.getValue();
    if (str == "true" || str == "false") return Bool;
    else if (legalNumber(str)) return Number;
    else if (str == "null") return Null;
    else if (legalString(str)) return String;
    return None;
}

bool JsonNode::legalNumber(const std::string &str) {
    char *pEnd = nullptr;
    std::strtod(str.c_str(), &pEnd);
    return pEnd != str.c_str();
}

bool JsonNode::legalString(const string &str) {
    if (str.size() < 2) return false;
    if (str[0] == str[str.size() - 1] && str[0] == '\"') return true;
    return false;
}

ostream &JsonNode::getString(ostream &os, uint32_t tab) {
    int32_t count = tab;
    while (count--)
        os << '\t';
    os << _key.getValue();
    if (_type == Array || _type == Object) {
        if (_key.getPtr()) os << " : ";
        os << (_type == Array ? "[" : "{");
    } else {
        if (_value.getPtr()) os << " : ";
        os << _value.getValue();
    }
    if (!_children.empty()) {
        os << endl;
        for (const auto &item : _children) {
            item->getString(os, tab + 1);
            if (item != _children.back()) os << ",";
            os << endl;
        }
    }
    if (_type == Array || _type == Object) {
        if (!_children.empty()) {
            count = tab;
            while (count--)
                os << '\t';
        }
        os << (_type == Array ? "]" : "}");
    }
    return os;
}

float JsonNode::toDouble(float defaultValue) const {
    assert(_type == Number);
    float result = defaultValue;
    stringstream ss(_value.getValue());
    ss >> result;
    return result;
}

double JsonNode::toDouble(double defaultValue) const {
    assert(_type == Number);
    double result = defaultValue;
    stringstream ss(_value.getValue());
    ss >> result;
    return result;
}

int32_t JsonNode::toInt32(int32_t defaultValue) const {
    assert(_type == Number);
    int32_t result = defaultValue;
    stringstream ss(_value.getValue());
    ss >> result;
    return result;
}

int64_t JsonNode::toInt32(int64_t defaultValue) const {
    assert(_type == Number);
    int64_t result = defaultValue;
    stringstream ss(_value.getValue());
    ss >> result;
    return result;
}

uint32_t JsonNode::toUInt32(uint32_t defaultValue) const {
    assert(_type == Number);
    uint32_t result = defaultValue;
    stringstream ss(_value.getValue());
    ss >> result;
    return result;
}

uint64_t JsonNode::toUInt64(uint64_t defaultValue) const {
    assert(_type == Number);
    uint64_t result = defaultValue;
    stringstream ss(_value.getValue());
    ss >> result;
    return result;
}

bool JsonNode::toBool() const {
    assert(_type == Bool);
    return strncmp(_value.getPtr(), "true", 4) == 0; }

char* JsonNode::toNull() const {
    assert(_type == Null);
    return "null";
}

NAME_SPACE_END()

// myUtil
