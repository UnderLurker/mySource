//
// Created by 常笑男 on 2023/10/15.
//

#include <sstream>
#include <fstream>
#include "JsonParse.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

char *myUtil::JsonBase::trim(const char *str) {
    JSON_ASSERT(str != JSON_NULL)
    char *result = JSON_NULL;
    try{
        size_t left = 0, right = strlen(str) - 1;
        while(*(str+left) == '\n' || *(str+left) == ' ' ||
                *(str+left) == '\r' || *(str+left) == '\t'){
            left++;
        }
        while(*(str+right) == '\n' || *(str+right) == ' ' ||
              *(str+right) == '\r' || *(str+right) == '\t'){
            right--;
        }
        JSON_ASSERT(left <= right)
        size_t len = right - left + 1;
        result = new char[len + 1];
        strncpy(result, str + left, len);
        result[len] = '\0';
    }catch(...){
        return result;
    }
    return result;
}

char *JsonBase::findSpecContext(char *source, const char *target, FindRange range) {
    JSON_ASSERT(source != JSON_NULL && target != JSON_NULL)
    char *result = JSON_NULL;
    try {
        char firstChar = *target;
        size_t quoteCount = 0;
        size_t pos = 0, targetLen = strlen(target), sourceLen = strlen(source);
        while(pos < sourceLen){
            bool quoteFlag = true;
            switch (range) {
                case Quote:{
                    quoteFlag = quoteCount%2;
                    break;
                }
                case NoQuote:{
                    quoteFlag = quoteCount%2 == 0;
                    break;
                }
                default:
                    break;
            }
            if(*(source + pos) == firstChar && quoteFlag &&
               strncmp(source + pos, target, targetLen) == 0)
                break;
            if(*(source + pos) == '\"')
                quoteCount++;
            pos++;
        }
        if(pos == sourceLen)
            return JSON_NULL;
        result = source + pos;
    }catch(...){
        return result;
    }
    return result;
}

JsonException JsonPair::reset() {
JsonException result = JSON_SUCCESS;

try{
    JSON_FREE(_key, true)
    JSON_FREE(_val, true)
    JSON_FREE_LINKLIST(_firstChild,false)
    _endChild = JSON_NULL;
    _deep = 0;
    _numChild = 0;
}catch(...){
    result = JSON_DESTRUCTION_ERROR;
}

return result;
}

JsonException JsonPair::setKey(const char *key) {
    JsonException result = JSON_SUCCESS;
    try{
        JSON_ASSERT(key!=JSON_NULL)
        JSON_FREE(_key, true)
        JSON_ASSERT(_key == JSON_NULL)

        _key = trim(key);
        JSON_TRIM_DETERMINE(_key)

    }catch(JsonException exception) {
        result = exception;
    }catch(...){
        result = JSON_MALLOC_OR_COPY_ERROR;
    }
    return result;
}

JsonException JsonPair::setValueText(const char *value) {
    JsonException result = JSON_SUCCESS;
    try{
        JSON_ASSERT(value!=JSON_NULL)
        JSON_FREE(_val, true)
        JSON_FREE_LINKLIST(_firstChild,false)
        _endChild = JSON_NULL;
        JSON_ASSERT(_val == JSON_NULL)

        _val = trim(value);
        JSON_TRIM_DETERMINE(_val)

        result = Parse();
    }catch(JsonException exception){
        result = exception;
    }catch(...){
        result = JSON_MALLOC_OR_COPY_ERROR;
    }
    return result;
}

JsonException JsonPair::setPairText(char* text){
    JsonException result = JSON_SUCCESS;
    try{
        JSON_ASSERT(text != JSON_NULL)
        if((result = reset()) != JSON_SUCCESS)
            throw result;
        JSON_ASSERT(_key == JSON_NULL && _val == JSON_NULL)

        char* next = findSpecContext(text,":",NoQuote);
        if(next == JSON_NULL)
            throw JSON_NOT_FIND;
        *next = '\0';
        _key = trim(text);
        _val = trim(next);
        JSON_TRIM_DETERMINE(_key)
        JSON_TRIM_DETERMINE(_val)
        Parse();
    }catch(JsonException exception) {
        result = exception;
    }catch(...){
        result = JSON_ERROR;
    }
    return result;
}

JsonException JsonPair::Parse() {
    JsonException result = JSON_SUCCESS;

    JSON_ASSERT(_val != JSON_NULL)
    char firstValChar = *_val;
    switch (firstValChar) {
        case '\"':{
            _type = String;
            break;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':{
            _type = Number;
            break;
        }
        case 'f':
        case 't':{
            _type = Bool;
            break;
        }
        case '[':
        case '{':{
            if(firstValChar == '[')
                _type = Array;
            else
                _type = Object;
            size_t valLen = strlen(_val);
            _val[valLen - 1] = '\0';
            char *start = _val + 1;
            char *targetPos = JSON_NULL;
            do{
                targetPos = findSpecContext(start,",",NoQuote);
                if(targetPos != JSON_NULL)
                    *targetPos = '\0';

                auto *pair = new JsonPair();
                char *temp = trim(start);
                pair->setPairText(temp);
                addChildPair(pair);

                JSON_FREE(temp, true)
                JSON_FREE(start, true);
                start = targetPos + strlen(",");
            }while(targetPos != JSON_NULL);//此处传入的是一整个，没有分为一个个的pair
            if(start != JSON_NULL){
                auto *pair = new JsonPair();
                char *temp = trim(start);
                pair->setPairText(temp);
                addChildPair(pair);

                JSON_FREE(temp, true)
                JSON_FREE(start, true)
            }
            start = JSON_NULL;
            _val = JSON_NULL;
            break;
        }
        default:
            result = JSON_PARSE_ERROR;
            break;
    }

    return result;
}

JsonException JsonPair::addNextPair(JsonPair *pair) {
    JsonException result = JSON_SUCCESS;
    try{
        JSON_ASSERT(pair!=JSON_NULL)
        JsonPair *insertPair = pair;
        while(insertPair->_nextPair!=JSON_NULL){
            insertPair = insertPair->_nextPair;
        }
        insertPair->_nextPair = _nextPair;
        _nextPair = pair;
    }catch(...){
        result = JSON_ERROR;
    }
    return result;
}

JsonException JsonPair::addChildPair(JsonPair *pair) {
    JsonException result = JSON_SUCCESS;
    try{
        JSON_ASSERT(pair!=JSON_NULL)
        if(_firstChild == JSON_NULL){
            _firstChild = pair;
            _endChild = pair;
        }
        else{
            _endChild->_nextPair = pair;
            _endChild = pair;
        }
    }catch(...){
        result = JSON_ERROR;
    }
    return result;
}

double JsonPair::parseDouble() const {
    JSON_ASSERT(_val != JSON_NULL)
    stringstream ss(_val);
    double result = 0;
    ss >> result;
    return result;
}

int JsonPair::parseInt() const {
    JSON_ASSERT(_val != JSON_NULL)
    stringstream ss(_val);
    int result = 0;
    ss >> result;
    return result;
}

bool JsonPair::parseBool() const {
    JSON_ASSERT(_val != JSON_NULL)
    bool result = false;
    if(strcmp(_val,"true") == 0)
        result = true;
    else
        result = false;
    return result;
}

char *JsonPair::parseString() const {
    JSON_ASSERT(_val != JSON_NULL)
    char *result = JSON_NULL;
    size_t len = strlen(_val);
    result = new char[len + 1];
    strcpy(result, _val);
    result[len] = '\0';
    return result;
}

template<class T>
T JsonPair::parseObject() const {
    JSON_ASSERT(_val == JSON_NULL)
    return T();
}

template<class T>
vector<T> JsonPair::parseList() const {
    JSON_ASSERT(_val == JSON_NULL)
    return vector<T>();
}

JsonParse::~JsonParse(){
    JSON_FREE(_cache, true);
}

std::ifstream::pos_type get_file_size(const std::string& filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

JsonException JsonParse::load(string filePath) {
    JsonException result = JSON_SUCCESS;
    if(filePath.length() != 0)
        _filePath = std::move(filePath);

    size_t totalLen = get_file_size(_filePath);
    _cache = new char[totalLen + 1];
    ifstream file(_filePath, ios::in);
    try{
        file.read(_cache, totalLen);
    }catch(...){
        result = JSON_READ_FILE_ERROR;
    }
    return result;
}

NAME_SPACE_END()



//myUtil