//
// Created by 常笑男 on 2023/10/15.
//

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

char *myUtil::JsonBase::findSpecContext(char *source, const char *target) {
    JSON_ASSERT(source != JSON_NULL && target != JSON_NULL)
    char *result = JSON_NULL;
    try {
        char firstChar = *target;
        size_t pos = 0, targetLen = strlen(target), sourceLen = strlen(source);
        while(pos < sourceLen){
            if(*(source + pos) == firstChar &&
                strncmp(source + pos, target, targetLen) == 0)
                break;
            pos++;
        }
        if(pos == sourceLen)
            return JSON_NULL;
        result = source + pos + targetLen;
    }catch(...){
        return result;
    }
    return result;
}

JsonException JsonPair::reset() {
    JsonException result = JSON_SUCCESS;

    try{
        JSON_FREE(_keyStart, true)
        JSON_FREE(_valStart, true)
        JSON_FREE(_nextPair, false)
        JSON_FREE_LINKLIST(_firstChild,false)
        _keyEnd = JSON_NULL;
        _valEnd = JSON_NULL;
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
        JSON_FREE(_keyStart, true)
        _keyEnd = JSON_NULL;
        JSON_ASSERT(_keyStart == JSON_NULL)

        size_t len = strlen(key);
        _keyStart = new char[len + 1];
        strncpy(_keyStart, key, len + 1);
        _keyEnd = _keyStart + len;
    }catch(...){
        result = JSON_MALLOC_OR_COPY_ERROR;
    }
    return result;
}

JsonException JsonPair::setValueText(const char *value, bool parse) {
    JsonException result = JSON_SUCCESS;
    try{
        JSON_ASSERT(value!=JSON_NULL)
        JSON_FREE(_valStart, true)
        _valEnd = JSON_NULL;
        JSON_ASSERT(_valStart == JSON_NULL)

        size_t len = strlen(value);
        _valStart = new char[len + 1];
        strncpy(_valStart, value, len + 1);
        _valEnd = _valStart + len;

        if(parse)
            result = Parse();
    }catch(...){
        result = JSON_MALLOC_OR_COPY_ERROR;
    }
    return result;
}

JsonException JsonPair::Parse() {
    JsonException result = JSON_SUCCESS;

    return result;
}


NAME_SPACE_END()


//myUtil