#pragma once
#include "Util.h"
#include <fstream>
#include <string>
#include <vector>
#ifndef _JSON_SERIALIZE_
#define _JSON_SERIALIZE_

NAME_SPACE_START(Json)

enum JsonType{
    Number,
    String,
    Array,
    Bool,
    Null,
    Object
};

class JBaseObject{
public:
    JBaseObject(){}
    virtual ~JBaseObject(){}
    virtual JBaseObject* clone() = 0;
};

template<typename T>
class JHolderObject:public JBaseObject{
private:
    T value;
public:
    JHolderObject(const T& val):value(val){}
    ~JHolderObject(){}
    JBaseObject* clone() override;
    T GetValue();
};

template<typename ValueType>
class JObject{
private:
    JBaseObject* _value;
public:
    JObject(const ValueType& value);
    JObject(const JObject& obj):_value(obj._value->clone()){}
    ~JObject();
    JObject<ValueType>& operator=(const ValueType& value);
    ValueType* Cast();
    ValueType& RefCast();
};

class JsonItemBase{
public:
    JsonItemBase(){}
    virtual ~JsonItemBase(){}
};

template<typename T>
class JsonItem : public JsonItemBase{
private:
    using string = std::string;
    JsonType _type;
    JObject<T> _value;
    string _key;
public:
    JsonItem(){}
    JsonItem(const JsonItem &);
    JsonItem(const JsonType,const string,const JObject<T>);
    ~JsonItem() = default;
    JsonType GetItemType();
    string GetItemKey();
    JObject<T> GetItemValue();
};

class JsonSerialize{
    using string = std::string;
private:
    std::vector<JsonItemBase> content;
    string _filePath;
public:
    JsonSerialize(){}
    JsonSerialize(const string filePath);
    ~JsonSerialize();
    bool Load(const string filePath = "");
    auto GetValueByKey(string);
    std::vector<JsonItemBase> GetContent();
    
private:
    
};


NAME_SPACE_END()
#endif //!_JSON_SERIALIZE_