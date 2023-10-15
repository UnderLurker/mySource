//
// Created by 常笑男 on 2023/10/15.
//

#include "JsonParse.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

JsonException JsonPair::reset() {
    JsonException result = JSON_SUCCESS;

    JSON_FREE(_keyStart, true);
    JSON_FREE(_valStart, true);
    _keyEnd = JSON_NULL;
    _valEnd = JSON_NULL;

    return result;
}

JsonException JsonPair::setKey(const char *key) {
    JsonException result = JSON_SUCCESS;
    try{
        JSON_ASSERT(key!=JSON_NULL)
        JSON_FREE(_keyStart, true);
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
        JSON_FREE(_valStart, true);
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