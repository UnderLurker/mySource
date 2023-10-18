//
// Created by 常笑男 on 2023/10/15.
//
// 最近看TinyXML2想了一个新的解析方式，写个新的文件来看看
// 本次重写加载文件使用缓存方式一次加载固定长度的字符串，分多次加载

#ifndef MAIN_JSONPARSE_H
#define MAIN_JSONPARSE_H

#include "Util.h"
#include <assert.h>

#include <utility>

NAME_SPACE_START(myUtil)

#define JSON_NULL nullptr
#define JSON_FREE(ptr,isArray)  \
        if(ptr!=JSON_NULL){     \
            if(isArray==true){  \
                delete [] ptr;  \
            }                   \
            else {              \
                delete ptr;     \
            }                   \
            ptr = JSON_NULL;    \
        }

#define JSON_FREE_LINKLIST(_firstChild,isArray) \
        auto ptr = _firstChild;                 \
        while(ptr != JSON_NULL){                \
            auto temp = ptr->_nextPair;         \
            JSON_FREE(ptr,isArray)              \
            ptr = temp;                         \
        }                                       \
        ptr = JSON_NULL;                        \
        _firstChild = JSON_NULL;

#define  JSON_TRIM_DETERMINE(charPtr)   \
        if(charPtr == JSON_NULL){       \
            throw JSON_TRIM_NULL_ERROR; \
        }

#define JSON_ASSERT(expression) assert(expression);

//-----------------------macro define start---------------
#define JSON_MAX_BUFFER_SIZE 4096
#define JSON_MAX_DEEP 500
//-----------------------macro define end-----------------

enum JsonException{
    JSON_SUCCESS = 0x0000,          // 成功
    JSON_NOT_FIND,                  // 查找指定字符返回为空
    JSON_ERROR,                     // 失败，不清楚原因
    JSON_MALLOC_OR_COPY_ERROR,      // 分配或者拷贝内存失败
    JSON_DESTRUCTION_ERROR,         // 析构失败
    JSON_TRIM_NULL_ERROR,           // trim字符串为空
    JSON_PARSE_ERROR,               // 解析val出错
    JSON_READ_FILE_ERROR,           // 读取文件内容出错
};

enum JsonPairType{
    Number,
    String,
    Array,
    Bool,
    Null,
    Object
};

// provide some basic functions
class JsonBase{
public:
    enum FindRange{
        All = 0x0,
        Quote = 0x01,
        NoQuote = 0x02
    };
    // remove spaces at beginning and end(include ' ', \t, \n and \r)
    virtual char* trim(const char* str);
    // search target in source, return to the last one at the end of the target
    virtual char* findSpecContext(char* source, const char* target, FindRange range);
    // to double
    virtual double parseDouble() const = 0;
    // to int
    virtual int parseInt() const = 0;
    // to bool
    virtual bool parseBool() const = 0;
    // to string
    virtual char* parseString() const = 0;
//    // to Object
//    template<class T>
//    T parseObject() const {}
//    // to object list
//    template<class T>
//    vector<T> parseList() const {}
};

//负责JSON中的每一对键值对，存储内容指针
class JsonPair : public JsonBase{
public:
    explicit JsonPair(): _key(JSON_NULL),
                         _val(JSON_NULL),
                         _nextPair(JSON_NULL),
                         _firstChild(JSON_NULL),
                         _endChild(JSON_NULL), JsonBase(){}

    ~JsonPair(){
        JSON_ASSERT(reset() == JSON_SUCCESS)
    }
    JsonException reset();

    // return ptr of key
    const char* getKey() const {return _key;};

    // set pair key with param
    JsonException setKey(const char* key);

    // return ptr of value
    char* getValueText() const {return _val;};

    // set pair value with param, and will use Parse function with parse equal true
    JsonException setValueText(const char* value);

    // set key and value, at the same time.
    JsonException setPairText(char* text);

    // analyze semantics of value, at the same time it also set _type's value
    JsonException Parse();

    JsonException addNextPair(JsonPair *pair);

    JsonException addChildPair(JsonPair *pair);

    double parseDouble() const override;
    int parseInt() const override;
    bool parseBool() const override;
    char* parseString() const override;
    template<class T>
    T parseObject() const;
    template<class T>
    vector<T> parseList() const;

private:

    char *_key;
    // if _type equals Object or Array, _val equals JSON_NULL
    char *_val;

    JsonPair* _nextPair;
    JsonPair* _firstChild;
    JsonPair* _endChild;
    JsonPairType _type;
    size_t _deep{0};
    size_t _numChild{0};

    JsonPair(const JsonPair&);
    JsonPair(const JsonPair&&) noexcept;
    JsonPair& operator=(const JsonPair&);
};

class JsonParse {
public:
    explicit JsonParse() : _filePath(JSON_NULL){}
    explicit JsonParse(string filePath) : _filePath(std::move(filePath)){}
    ~JsonParse();

    JsonException load(string filePath = "");
private:
    // json文件路径
    string _filePath;
    // json数据缓存
    char* _cache;
};

NAME_SPACE_END() //myUtil

#endif //MAIN_JSONPARSE_H
