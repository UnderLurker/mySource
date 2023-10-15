//
// Created by 常笑男 on 2023/10/15.
//
// 最近看TinyXML2想了一个新的解析方式，写个新的文件来看看
// 本次重写加载文件使用缓存方式一次加载固定长度的字符串，分多次加载

#ifndef MAIN_JSONPARSE_H
#define MAIN_JSONPARSE_H

#include "Util.h"
#include <assert.h>

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
#define JSON_ASSERT(expression) assert(expression);
#define JSON_MAX_BUFFER_SIZE 4096

enum JsonException{
    JSON_SUCCESS = 0x0001,
    JSON_ERROR = 0x0002,
    JSON_MALLOC_OR_COPY_ERROR = 0x0003,
};

enum JsonPairType{
    Number,
    String,
    Array,
    Bool,
    Null,
    Object
};

//负责JSON中的每一对键值对，存储内容指针
class JsonPair{
public:
    explicit JsonPair(): _keyStart(JSON_NULL),
                         _keyEnd(JSON_NULL),
                         _valStart(JSON_NULL),
                         _valEnd(JSON_NULL){}

    ~JsonPair(){
        JSON_FREE(_keyStart, true);
        JSON_FREE(_valStart, true);
        _keyEnd = JSON_NULL;
        _valEnd = JSON_NULL;
    }
    JsonException reset();

    // return ptr of key
    const char* getKey() const {return _keyStart;};

    // set pair key with param
    JsonException setKey(const char* key);

    // return ptr of value
    char* getValueText() const {return _valStart;};

    // set pair value with param, and will use Parse function with parse equal true
    JsonException setValueText(const char* value, bool parse = true);

    // analyze semantics, at the same time it also set _type's value
    JsonException Parse();
private:
    char *_keyStart;
    char *_keyEnd;
    char *_valStart;
    char *_valEnd;
    JsonPairType _type;
};

class JsonParse {

};

NAME_SPACE_END() //myUtil

#endif //MAIN_JSONPARSE_H
