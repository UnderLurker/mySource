#pragma once
#include <cstdint>
#include <type_traits>
#ifndef _JSON_SERIALIZE_
#define _JSON_SERIALIZE_
#include "Util.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
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
    template<typename T>
    class JsonItem;

    class JNull {
    public:
        JNull() = default;
        ~JNull() = default;
    };


    template<typename T>
    struct JsonItem_traits{};

    template<>
    struct JsonItem_traits<JNumber>{
        using value_type = double*;
    };

    template<>
    struct JsonItem_traits<JString>{
        using value_type = wchar_t*;
        // using value_type = JsonItem<wchar_t*>;
    };

    template<>
    struct JsonItem_traits<JNull>{
        using value_type = JNull*;
    };

    template<>
    struct JsonItem_traits<JBool>{
        using value_type = bool*;
    };

    template<>
    struct JsonItem_traits<JObj>{
        using value_type = JsonSerialize*;
    };

    template<>
    struct JsonItem_traits<JArray>{
        using value_type = std::vector<JsonSerialize*>*;
    };

    template<typename T>
    using JsonItem_t = typename JsonItem_traits<T>::value_type;

    class JsonKey {
    public:
        JsonType _type{ JsonType::None };
        std::wstring _key{ L"" };
        JsonKey() = default;
        JsonKey(const std::wstring& key) : _key(key) {}
        JsonKey(const JsonType& type, const std::wstring& key) :_type(type), _key(key) {}
        bool operator<(const JsonKey&) const;
    };

    class JsonSerialize {
    private:
        map<JsonKey, void*> content;
        string _filePath;
    public:
        JsonSerialize() = default;
        explicit JsonSerialize(const string& filePath);
        ~JsonSerialize();
        bool Load(const string& filePath = "");
        map<JsonKey, void*>& GetContent();

        template<typename T>
        JsonItem_t<T> GetValueByKey(const std::wstring& key) const;

        template<typename T>
        bool setValueByKey(const std::wstring& key, JsonItem_t<T> value);

        bool insert(const JsonType& type, const wstring& key, void* value);

        void printAll(int tab = 0);
    private:
    };

NAME_SPACE_END()

#include "../src/JsonSerialize.cpp"

#endif //!_JSON_SERIALIZE_