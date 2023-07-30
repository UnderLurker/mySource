#pragma once
#ifndef _JSON_SERIALIZE_
#define _JSON_SERIALIZE_
#include "Util.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

NAME_SPACE_START(Json)

enum JsonType {
    None,
    Number,
    String,
    Array,
    Bool,
    Null,
    Object
};

class JNull {
public:
    JNull() = default;
    ~JNull() = default;
};

template<typename T>
class JBaseObject {
public:
    JBaseObject() = default;
    virtual ~JBaseObject() = default;
    virtual JBaseObject* clone() = 0;
};

template<typename T>
class JHolderObject :public JBaseObject<T> {
private:
public:
    T value;
    explicit JHolderObject(const T& val) :value(val) {}
    ~JHolderObject() = default;
    JBaseObject<T>* clone() override;
};

template<typename ValueType>
class JObject {
private:
    JBaseObject<ValueType>* _value = nullptr;
public:
    JObject() = default;
    explicit JObject(const ValueType& value);
    JObject(const JObject& obj) :_value(obj._value->clone()) {}
    ~JObject();
    JObject<ValueType>& operator=(const ValueType& value);
    ValueType* Cast() {
        JHolderObject<ValueType>* temp = dynamic_cast<JHolderObject<ValueType>*>(_value);
        return temp ? &temp->value : NULL;
    }
    ValueType& RefCast() {
        return (static_cast<JHolderObject<ValueType>*>(_value))->value;
    }
};

template<typename T>
class JsonItemBase {
public:
    JsonItemBase() = default;
    virtual ~JsonItemBase() = default;
};

template<typename T>
class JsonItem {
private:
    // JObject<T> _value;
    T _value;
public:
    JsonItem() = default;
    JsonItem(const JsonItem&) = default;
    explicit JsonItem(const T& obj);
    ~JsonItem() = default;
    T& GetItemRefValue() {
        return this->_value;
    }
    T* GetItemLpValue() {
        return &this->_value;
    }
};

template<typename T>
class JsonItem<T*>{
private:
    T* _value{nullptr};
public:
    JsonItem(const JsonItem&) = delete;
    explicit JsonItem(T* obj) : _value(obj){}
    ~JsonItem(){
        if(_value!= nullptr) delete _value;
    }
    T& GetItemRefValue() {
        return *(this->_value);
    }
    T* GetItemLpValue() {
        return this->_value;
    }
};

class JsonKey {
public:
    JsonType _type{ JsonType::None };
    std::wstring _key{ L"" };
    JsonKey() = default;
    JsonKey(const JsonType& type, const std::wstring& key) :_type(type), _key(key) {}
    bool operator<(const JsonKey&) const;
};

class JsonValue {
public:
    void* _value{ nullptr };
    size_t _size{ 0 };
    JsonType _type{JsonType::None};
    JsonValue() = default;
    JsonValue(void* value, const int& size) :_value(value), _size(size) {}
    ~JsonValue(){
//        if(_value!=nullptr){
//            if(Json)
//        }
    }
};

class JsonSerialize {
private:
    std::vector<std::pair<JsonKey, JsonValue>> content;
    string _filePath;
public:
    JsonSerialize() = default;
    explicit JsonSerialize(const string& filePath);
    ~JsonSerialize() = default;
    bool Load(const string& filePath = "");

    template<typename T>
    JsonItem<T>* GetValueByKey(const std::wstring& key);
    std::vector<std::pair<JsonKey, JsonValue>>& GetContent();
    void printAll(int tab = 0);
private:

};


template<typename T>
JBaseObject<T>* JHolderObject<T>::clone() {
    return new JHolderObject<T>(this->value);
}

template<typename ValueType>
JObject<ValueType>::JObject(const ValueType& value) {
    _value = new JHolderObject<ValueType>(value);
}

template<typename ValueType>
JObject<ValueType>::~JObject() {
    if (_value != nullptr) delete _value;
}

template<typename ValueType>
JObject<ValueType>& JObject<ValueType>::operator=(const ValueType& value) {
    if (_value) delete _value;
    _value = new JHolderObject<ValueType>(value);
    return *this;
}

template<typename T>
JsonItem<T>::JsonItem(const T& objList)
{
    this->_value = objList;
}

template<typename T>
JsonItem<T>* JsonSerialize::GetValueByKey(const std::wstring& key)
{
    JsonItem<T>* temp_value = nullptr;
    for (auto item : this->content) {
        if (item.first._key == key) {
            temp_value = (JsonItem<T>*)(item.second._value);
            break;
        }
    }
    return temp_value;
}

NAME_SPACE_END()

#include "../src/JsonSerialize.cpp"

#endif //!_JSON_SERIALIZE_