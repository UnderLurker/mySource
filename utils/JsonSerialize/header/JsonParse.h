//
// Created by 常笑男 on 2023/10/15.
//
// 最近看TinyXML2想了一个新的解析方式，写个新的文件来看看
// 本次重写加载文件使用缓存方式一次加载固定长度的字符串，分多次加载

#ifndef _JSONPARSE_H
#define _JSONPARSE_H

// #define _JSON_PARSE_DEBUG

#include <cassert>
#include <list>
#include <utility>

#include "Util.h"

NAME_SPACE_START(myUtil)

enum JsonType {
    None   = 0,
    Number = 1,
    String = 2,
    Array  = 4,
    Bool   = 8,
    Null   = 16,
    Object = 32
};

enum JsonStatus {
    SUCCESS = 0,
    ERROR_FILE_READ,
    ERROR_FORMAT,
    ERROR_NULLPTR
};

class JsonNode;
class JsonKey;
class JsonValue;

class JsonDocument {
public:
    JsonDocument() = default;
    explicit JsonDocument(const char *filePath);
    ~JsonDocument();

    JsonStatus Load(const char *filePath);
    [[nodiscard]] JsonNode *getNode() const { return _node; }
    JsonNode *CreateNode();
    void save(const char *filePath = nullptr);

private:
    JsonStatus Analysis();

private:
    char *_cache {nullptr};    // 存储所有初始化读取的数据
    char *_filePath {nullptr}; // 存储文件路径
    size_t _length {0};        // _cache长度
    JsonNode *_node {nullptr}; // 代表整个json文件的最外层节点
    list<char *> _memoryPool; // 用来存储所有后插入的数据，其他指针所指向的位置都在此对象中
};

class JsonStr {
public:
    JsonStr() = default;
    JsonStr(const JsonStr &obj) {
        _ptr    = obj._ptr;
        _length = obj._length;
    }
    JsonStr(JsonStr &&obj) noexcept {
        _ptr        = obj._ptr;
        _length     = obj._length;
        obj._ptr    = nullptr;
        obj._length = 0;
    }
    JsonStr(char *start, size_t length)
        : _ptr(start), _length(length) {}
    virtual ~JsonStr() {
        _ptr    = nullptr;
        _length = 0;
    }

    void setValue(char *start, size_t length);
    [[nodiscard]] string getValue() const;
    void setPtr(char *ptr) { _ptr = ptr; }
    [[nodiscard]] const char *getPtr() const { return _ptr; }
    void setLength(size_t length) { _length = length; }
    [[nodiscard]] size_t getLength() const { return _length; }

    JsonStr &operator=(const JsonStr &obj) = default;

private:
    char *_ptr {nullptr};
    size_t _length {0};
};

class JsonNode {
public:
    JsonNode() = default;
    JsonNode(JsonNode &&obj) noexcept;
    explicit JsonNode(char *&begin, JsonDocument *doc, JsonNode *parent);
    ~JsonNode() = default;

    JsonStatus Load(char *&begin);
    void setDocument(JsonDocument *doc) noexcept {
        CHECK_NULL_VOID(doc);
        _doc = doc;
    }

    void setKey(char *begin, size_t length) { _key.setValue(begin, length); }
    void setValue(char *begin, size_t length) { _value.setValue(begin, length); }
    void setNext(JsonNode *node) { _next = node; }
    [[nodiscard]] string getKey() const { return _key.getValue(); }
    [[nodiscard]] string getValue() const { return _value.getValue(); }
    [[nodiscard]] JsonStatus getStatus() const { return _status; }
    [[nodiscard]] JsonNode *getNode() const { return _next; }

    [[nodiscard]] float toDouble(float defaultValue = 0) const;
    [[nodiscard]] double toDouble(double defaultValue = 0) const;
    [[nodiscard]] int32_t toInt32(int32_t defaultValue = 0) const;
    [[nodiscard]] int64_t toInt32(int64_t defaultValue = 0) const;
    [[nodiscard]] uint32_t toUInt32(uint32_t defaultValue = 0) const;
    [[nodiscard]] uint64_t toUInt64(uint64_t defaultValue = 0) const;
    [[nodiscard]] bool toBool() const;
    [[nodiscard]] char* toNull() const;

    ostream &getString(ostream &os, uint32_t tab = 0);

private:
    JsonStatus parseValue(char *&begin, const char *end);

    /**
     * 只用来处理非object，array类型
     */
    static JsonStr trim(char *&begin);
    static void runSpace(char *&begin);
    static JsonType determineType(const JsonStr &input);
    static bool legalNumber(const string &str);
    static bool legalString(const string &str);

private:
    JsonStr _key;
    JsonStr _value;
    JsonType _type {None};
    JsonStatus _status {SUCCESS};
    list<JsonNode *> _children;
    JsonNode *_next {nullptr};
    JsonNode *_parent {nullptr};
    JsonDocument *_doc {nullptr};
};

NAME_SPACE_END() // myUtil

#endif // _JSONPARSE_H
