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

JsonDocument::JsonDocument(const char* filePath)
{
    Load(filePath);
}

JsonDocument::~JsonDocument()
{
    delete[] _cache;
    delete[] _filePath;
    _cache = nullptr;
    _filePath = nullptr;
}

JsonStatus JsonDocument::Load(const char* filePath)
{
    delete[] _filePath;
    uint32_t length = strlen(filePath);
    _filePath = new char[length + 1];
    memcpy_s(_filePath, sizeof(char) * length, filePath, length);
    _filePath[length] = '\0';

    fstream file(filePath, ios::in | ios::binary);
    if (file.fail())
        return ERROR_FILE_READ;
    file.seekg(0, fstream::end);
    size_t endPos = file.tellg();
    file.seekg(0, fstream::beg);
    _cache = new char[endPos + 1];
    _length = endPos + 1;
    file.read(_cache, endPos);
    _cache[endPos] = '\0';
    return Analysis();
}

JsonStatus JsonDocument::Analysis()
{
    auto tmp = _cache;
    _node = new JsonNode(tmp, this, nullptr);
    return _node->getStatus();
}

////////////////////////////////////////////////////
//               JsonStringBase                   //
////////////////////////////////////////////////////
void JsonStr::setValue(char* start, size_t length)
{
    _ptr = start;
    _length = length;
}

const string JsonStr::getValue() const
{
    return string(_ptr, _length);
}

////////////////////////////////////////////////////
JsonNode::JsonNode(JsonNode&& obj) noexcept
{
    _key = obj._key;
    _value = obj._value;
    _type = obj._type;
    _status = obj._status;
    _children = obj._children;
    _next = obj._next;
    _parent = obj._parent;
    _doc = obj._doc;
    obj._key.setValue(nullptr, 0);
    obj._value.setValue(nullptr, 0);
    obj._children.clear();
    obj._next = nullptr;
    obj._parent = nullptr;
    obj._doc = nullptr;
}

JsonNode::JsonNode(char*& begin, JsonDocument* doc, JsonNode* parent) : _doc(doc), _parent(parent)
{
    _status = Load(begin);
}

JsonStatus JsonNode::Load(char*& begin)
{
    size_t len = strlen(begin);
    char* end = begin + len;
    bool isKey = false;
    // 先看:前面的值，可能没有:
    while (begin < end && (*begin == '\r' || *begin == '\n' || *begin == ' '))
        begin++;
    switch (*begin) {
        case '{':
        case '[': {
            _type = *begin == '{' ? Object : Array;
            _children.emplace_back(new JsonNode(++begin, _doc, this));
            break;
        }
        case '}':
        case ']':{
            begin++;
            return SUCCESS;
        }
        case '"': {
            auto l = begin++;
            while (begin < end && *begin != '"')
                begin++;
            _key.setValue(l, begin - l + 1);
            cout << "key:" << _key.getValue() << endl;
            isKey = true;
            break;
        }
        default:
            return ERROR_FORMAT;
    }
    begin++;
    if (!isKey)
        return Load(begin);
    JsonStatus status = parseValue(begin, end);
    if (status != SUCCESS)
        return status;
    return Load(begin);
}

JsonStatus JsonNode::parseValue(char*& begin, const char* end)
{
    CHECK_NULL_RETURN(begin, ERROR_NULLPTR)
    CHECK_NULL_RETURN(end, ERROR_NULLPTR)
    while (begin < end && *begin == ' ')
        begin++;
    if (*begin == ':')
        begin++;
    else if (*begin == ',')
        return SUCCESS;
    runSpace(begin);
    switch (*begin) {
        case '{':
        case '[': {
            _type = *begin == '{' ? Object : Array;
            _children.emplace_back(new JsonNode(++begin, _doc, this));
            break;
        }
        default: {
            _value = JsonNode::trim(begin);
            cout << "value:" << _value.getValue() << endl;
            _type = determineType(_value);
            if (_type == None)
                return ERROR_FORMAT;
        }
    }
    return SUCCESS;
}

JsonStr JsonNode::trim(char*& begin)
{
    size_t len = strlen(begin);
    char* end = begin + len;
    while (begin < end && *begin == ' ')
        begin++;
    JsonStr result(begin, 0);
    while (begin < end && *begin != '\n')
        begin++;
    auto l = begin;
    while (*l == '\n' || *l == '\r' || *l == ' ' || *l == ',')
        l--;
    result.setLength(l - result.getPtr() + 1);
    begin++;
    return result;
}

void JsonNode::runSpace(char*& begin)
{
    while(*begin == '\r' || *begin == '\n' || *begin == ' ')
        begin++;
}

JsonType JsonNode::determineType(const JsonStr& input)
{
    string str = input.getValue();
    if (str == "true" || str == "false")
        return Bool;
    else if (legalNumber(str))
        return Number;
    else if (str == "null")
        return Null;
    else if (legalString(str))
        return String;
    return None;
}

bool JsonNode::legalNumber(const std::string& str)
{
    char* pEnd = nullptr;
    std::strtod(str.c_str(), &pEnd);
    return pEnd != str.c_str();
}

bool JsonNode::legalString(const string& str)
{
    if (str.size() < 2)
        return false;
    if (str[0] == str[str.size() - 1] && str[0] == '\"')
        return true;
    return false;
}

NAME_SPACE_END()

// myUtil