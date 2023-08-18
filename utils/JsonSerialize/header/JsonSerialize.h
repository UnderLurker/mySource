#pragma once
#include <cstdint>
#include <type_traits>
#include <assert.h>
#ifndef _JSON_SERIALIZE_
#define _JSON_SERIALIZE_
#include "Util.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

NAME_SPACE_START(myUtil)

    enum JsonType {
        None=0,
        Number,
        String,
        Array,
        Bool,
        Null,
        Object
    };

    struct JNumber{};
    struct JString{};
    struct JArray{};
    struct JBool{};
    struct JObj{};

    class JsonValue;
    class JsonSerialize;

    class JsonNumber;
    class JsonString;
    class JsonNull;
    class JsonBool;
    class JsonObj;
    class JsonArray;

    class JNull {
    public:
        JNull() = default;
        ~JNull() = default;
    };

    wchar_t* str2lp(const wstring& input);

    template<typename T>
    struct JsonItem_traits{};

    template<>
    struct JsonItem_traits<JNumber>{
        using value_type = JsonNumber;
        using target_type = double;
    };

    template<>
    struct JsonItem_traits<JString>{
        using value_type = JsonString;
        using target_type = wstring;
    };

    template<>
    struct JsonItem_traits<JNull>{
        using value_type = JsonNull;
        using target_type = void;
    };

    template<>
    struct JsonItem_traits<JBool>{
        using value_type = JsonBool;
        using target_type = bool;
    };

    template<>
    struct JsonItem_traits<JObj>{
        using value_type = JsonObj;
        using target_type = JsonSerialize;
    };

    template<>
    struct JsonItem_traits<JArray>{
        using value_type = JsonArray;
        using target_type = JsonArray;
    };

    template<typename T>
    using JsonItem_t = typename JsonItem_traits<T>::value_type*;

    template<typename T>
    using JsonItem_t_type = typename JsonItem_traits<T>::target_type;

    class JsonKey {
    public:
        JsonType _type{ JsonType::None };
        std::wstring _key{ L"" };
        JsonKey() = default;
        JsonKey(const std::wstring& key) : _key(key) {}
        JsonKey(const JsonType& type, const std::wstring& key) :_type(type), _key(key) {}
        bool operator<(const JsonKey&) const;
        bool operator==(const JsonKey& val) const{ return _type==val._type&&_key==val._key; }
        bool operator!=(const JsonKey& val) const{ return _type!=val._type||_key!=val._key; }
        JsonKey& operator=(const JsonKey& val) = default;
    };

    class JsonValue{
    public:
        virtual ~JsonValue() = default;
        virtual wstring toString() = 0;
    };

    class JsonNumber : public JsonValue{
    private:
        double value{0};
    public:
        JsonNumber() = default;
        JsonNumber(const JsonNumber& obj){ value=obj.value; }
        explicit JsonNumber(const double& val) : value(val){}
        ~JsonNumber() override = default;
        explicit operator double() const{ return value; }
        bool operator==(const JsonNumber& val) const{ return value==val.value; }
        JsonNumber& operator=(const JsonNumber& val){
            this->value=val.value;
            return *this;
        }
        wstring toString() override{
            wstringstream wss;
            wss<<value;
            wstring res;
            wss>>res;
            return res;
        }
    };

    class JsonString : public JsonValue{
    private:
        wstring value;
    public:
        JsonString() = default;
        JsonString(const JsonString& obj){ value=obj.value; }
        explicit JsonString(wstring val) : value(std::move(val)){}
        ~JsonString() override = default;
        explicit operator wstring() const{return value;}
        bool operator==(const JsonString& val) const {return value==val.value; }
        JsonString& operator=(const JsonString& val){
            this->value=val.value;
            return *this;
        }
        wstring toString() override{
            return value;
        }
    };

    class JsonNull : public JsonValue{
    public:
        JsonNull() = default;
        JsonNull(const JsonNull&) = default;
        ~JsonNull() override = default;
        bool operator==(const JsonNull& val) const { return true; }
        wstring toString() override{ return L"";}
    };

    class JsonBool : public JsonValue{
    private:
        bool value{false};
    public:
        JsonBool() = default;
        JsonBool(const JsonBool& obj){ value=obj.value; }
        explicit JsonBool(const bool& val) : value(val){}
        ~JsonBool() override = default;
        explicit operator bool() const { return value; }
        bool operator==(const JsonBool& val) const { return value==val.value; }
        JsonBool& operator=(const JsonBool& val){
            this->value=val.value;
            return *this;
        }
        wstring toString() override{
            return value?L"true":L"false";
        }
    };

    class JsonSerialize {
    private:
        map<JsonKey, JsonValue*> content;
        string _filePath;
    public:
        JsonSerialize() = default;
        JsonSerialize(const JsonSerialize& val);
        explicit JsonSerialize(const string& filePath);
        ~JsonSerialize();
        bool Load(const string& filePath = "");
        map<JsonKey, JsonValue*>& GetContent() { return content; }

        template<typename T>
        JsonItem_t_type<T> GetValueByKey(const std::wstring& key) const;

        bool setValueByKey(const std::wstring& key, JsonValue* value);

        bool insert(const JsonType& type, const wstring& key, JsonValue* value);

        void printAll(int tab = 0);

        bool operator==(const JsonSerialize& val);

        JsonSerialize& operator=(const JsonSerialize& val);

        wstring toString();

        bool saveJson(const string& filePath);

    private:
    };


    class JsonObj : public JsonValue{
    private:
        JsonSerialize value;
    public:
        JsonObj() = default;
        JsonObj(const JsonObj& val) {value = val.value;}
        explicit JsonObj(const JsonSerialize& val) : value(val){}
        ~JsonObj() override = default;
        explicit operator JsonSerialize() const { return value; }
        bool operator==(const JsonObj& val) {return value==val.value;}
        JsonObj& operator=(const JsonObj& val) {this->value=val.value; return *this;}

        void printAll(int tab = 0){value.printAll(tab);}

        map<JsonKey, JsonValue*>& GetContent() { return value.GetContent(); }

        bool insert(const JsonType& type, const wstring& key, JsonValue* valueLp){ return value.insert(type,key,valueLp);}

        template<typename T>
        JsonItem_t_type<T> GetValueByKey(const std::wstring& key) const{ return value.GetValueByKey<T>(key); }

        bool setValueByKey(const std::wstring& key, JsonValue* valueLp){ return value.setValueByKey(key,valueLp);}
        wstring toString() override { return value.toString(); }
    };

    class JsonArray : public JsonValue{
    private:
        vector<JsonObj> value;
        using iterator =vector<JsonObj>::iterator;
        using const_iterator = vector<JsonObj>::const_iterator;
        using reverse_iterator = vector<JsonObj>::reverse_iterator;
    public:
        JsonArray() = default;
        JsonArray(const JsonArray& obj);
        explicit JsonArray(const vector<JsonObj>& val){
            value.clear();
            value = vector<JsonObj>(val.begin(),val.end());
        }
        ~JsonArray() override = default;
        size_t size() const noexcept {return value.size();}
        JsonObj& operator[](size_t index){
//            assert(index<0||index>=value.size());
            return value.at(index);
        }
        iterator begin() noexcept {return value.begin();}
        iterator end() noexcept {return value.end();}
        const_iterator begin() const noexcept {return value.begin();}
        const_iterator end() const noexcept {return value.end();}
        const_iterator cbegin() const noexcept {return value.cbegin();}
        const_iterator cend() const noexcept {return value.cend();}
        reverse_iterator rbegin() noexcept {return value.rbegin();}
        reverse_iterator rend() noexcept {return value.rend();}
        void push_back(const JsonObj& val){ value.push_back(val);}
        bool operator==(const JsonArray& val);
        JsonArray& operator=(const JsonArray& val);
        wstring toString() override{
            wstring res=L"[";
            for(auto item : value) {
                res += item.toString() + L",\n";
            }
            return res.substr(0,res.size()-2)+L"]";
        }

    };


NAME_SPACE_END()

#include "../src/JsonSerialize.cpp"

#endif //!_JSON_SERIALIZE_