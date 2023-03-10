#pragma once
#ifndef _JSON_SERIALIZE_
#define _JSON_SERIALIZE_
// #include "Util.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>

// NAME_SPACE_START(Json)

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
    JNull() {}
    ~JNull() {}
};

template<typename T>
class JBaseObject {
public:
    JBaseObject() {}
    virtual ~JBaseObject() {}
    virtual JBaseObject* clone() = 0;
};

template<typename T>
class JHolderObject :public JBaseObject<T> {
private:
public:
    T value;
    JHolderObject(const T& val) :value(val) {}
    ~JHolderObject() {}
    JBaseObject<T>* clone() override;
};

template<typename ValueType>
class JObject {
private:
    JBaseObject<ValueType>* _value = nullptr;
public:
    JObject() {}
    JObject(const ValueType& value);
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
    JsonItemBase() {}
    virtual ~JsonItemBase() {}
    virtual void f() {}
};

template<typename T>
class JsonItem {
private:
    // JObject<T> _value;
    T _value;
public:
    JsonItem() {}
    JsonItem(const JsonItem&);
    JsonItem(const JObject<T>& obj);
    JsonItem(const T& obj);
    ~JsonItem() {};
    T& GetItemRefValue() {
        return this->_value;
    }
    T* GetItemLpValue() {
        return &this->_value;
    }
};

class JsonKey {
public:
    JsonType _type{ JsonType::None };
    std::wstring _key{ L"" };
    JsonKey() {}
    JsonKey(const JsonType& type, const std::wstring& key) :_type(type), _key(key) {}
    bool operator<(const JsonKey&) const;
};

class JsonValue {
public:
    void* _value{ nullptr };
    size_t _size{ 0 };
    JsonValue() {}
    JsonValue(void* value, const int& size) :_value(value), _size(size) {}
};

class JsonSerialize {
    using string = std::string;
private:
    std::vector<std::pair<JsonKey, JsonValue>> content;
    string _filePath;
public:
    JsonSerialize() {}
    JsonSerialize(const string filePath);
    ~JsonSerialize() {};
    bool Load(const string filePath = "");
    template<typename T>
    JsonItem<T>* GetValueByKey(string key);
    std::vector<std::pair<JsonKey, JsonValue>>& GetContent();
    void printAll(int tab = 0);
private:

};


// NAME_SPACE_END()
#endif //!_JSON_SERIALIZE_
