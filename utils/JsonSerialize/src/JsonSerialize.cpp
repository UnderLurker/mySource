#include "Util.h"
#include <iostream>
#include <sstream>
#include <exception>
#include <fstream>
#include <ios>
#include <string>
#include <utility>
#include <vector>
#include <stack>
#include <algorithm>
#include "JsonSerialize.h"

NAME_SPACE_START(myUtil)

    std::wifstream _file;
    void OpenFile(const std::string filePath) {
        _file.open(filePath, std::ios::in);
        if (!_file.is_open()) return;
        //std::locale china("zh_CN.UTF-8");
        //_file.imbue(china);
    }
    void CloseFile() {
        _file.close();
    }

    void removeValue(const JsonType& type, void* lpValue){
        switch (type) {
            case Number:{
                delete (JsonItem_t<JNumber>*)lpValue;
                break;
            }
            case String:{
                delete (JsonItem_t<JString>*)lpValue;
                break;
            }
            case Array:{
                delete (JsonItem_t<JArray>*)lpValue;
                break;
            }
            case Bool:{
                delete (JsonItem_t<JBool>*)lpValue;
                break;
            }
            case Null:{
                delete (JsonItem_t<JNull>*)lpValue;
                break;
            }
            case Object:{
                delete (JsonItem_t<JObj>*)lpValue;
                break;
            }
            default:
                break;
        }
        lpValue=nullptr;
    }

    bool JsonKey::operator<(const JsonKey& j_key) const {
        wstring key1=j_key._key,key2=_key;
        transform(key1.begin(),key1.end(),key1.begin(), ::tolower);
        transform(key2.begin(),key2.end(),key2.begin(),::tolower);
        return key1>key2;
    }

    JsonSerialize::JsonSerialize(const string& filePath) {
        this->_filePath = filePath;
    }

    JsonSerialize::~JsonSerialize(){
        for(auto item : content){
            removeValue(item.first._type,item.second);
        }
    }

    bool isFirst = true;
    bool isQuote = false;
    bool isKey = true;
    bool isObj = false;
    bool isObjEnd = true;
    bool isArrEnd = true;
    JsonType curType = JsonType::Null;
    size_t objListDeep = 0;
    size_t arrDeep = 0;
    std::wstring key = L"";
    std::wstring value = L"";
    std::stack<std::wstring> arrKey;
    std::stack<JsonType> cer;
    std::vector<std::pair<bool,std::vector<JsonSerialize*>*>> *arrValueList =
            new std::vector<std::pair<bool,std::vector<JsonSerialize*>*>>(); //false直接放到对象中 true 放到objList中
    std::vector<std::pair<bool,JsonSerialize*>> *objList =
            new std::vector<std::pair<bool,JsonSerialize*>>();               //false直接放到对象中 true 放到arrValueList中

    void freeAll() {
        for (std::pair<bool, std::vector<JsonSerialize*>*> item : *arrValueList)
            if (item.second!= nullptr) delete item.second;
        for (std::pair<bool, JsonSerialize*> item : *objList)
            if (item.second!= nullptr) delete item.second;
    }

    bool isBool(const std::wstring& str) {
        return str == L"true";
    }

    bool isNumber(const std::wstring& str) {
        std::wstringstream ss(str);
        double d;
        wchar_t c;
        if (!(ss >> d)) return false;
        if (ss >> c) return false;
        return true;
    }

    bool Analysis(const wchar_t* buffer, std::map<JsonKey, void*>& content, const size_t length) {
        size_t charLength = wcslen(buffer) < length ? wcslen(buffer) : length;
        for (size_t i = 0; i < charLength; i++) {
            wchar_t ch = buffer[i];
            if (ch == '{' && !isQuote) {
                objListDeep++;
                curType = JsonType::Object;
                if (objListDeep >= 2) {
                    bool flag = !(cer.empty() || cer.top() == JsonType::Object);
                    objList->push_back(std::pair<bool, JsonSerialize*>(flag, new JsonSerialize()));
                    isObj = true;
                    isObjEnd = false;
                    //if (!isArrEnd) cer.push(JsonType::Array);
                    cer.push(JsonType::Object);
                    if (isKey) arrKey.emplace(L"");
                }
            }
            else if (ch == '}' && !isQuote) {
                if (objListDeep == 1 && isFirst) isFirst = false;
                else if (objListDeep == 1 && !isFirst) return false;
                objListDeep--;
                isObjEnd = true;
                curType = JsonType::Object;
                if (objListDeep != 0)
                    goto addArray;
            }
            else if (ch == '[' && !isQuote) {
                bool flag = !(cer.empty() || cer.top() == JsonType::Object);
                curType = JsonType::Array;
                arrDeep++;
                arrValueList->push_back(std::pair<bool, std::vector<JsonSerialize*>*>(flag,new std::vector<JsonSerialize*>()));
                isArrEnd = false;
                //if (!isObjEnd) cer.push(JsonType::Object);
                cer.push(JsonType::Array);
            }
            else if (ch == ']' && !isQuote) {
                arrDeep--;
                curType = JsonType::Array;
                if (arrDeep == 0) isArrEnd = true;
                if(!key.empty()){
                    arrKey.push(key);
                    key=L"";
                    isKey=true;
                }
                goto addArray;
            }
            else if (ch == ' ' && !isQuote) continue;
            else if (ch == '"' && !isQuote) {
                isQuote = true;
                if (isKey) key += L"\"";
                else value += L"\"";
            }
            else if (ch == '"' && isQuote) {
                isQuote = false;
                if (isKey) key += L"\"";
                else value += L"\"";
            }
            else if (ch == ':' && !isQuote) isKey = false;
            else if ((ch == ',' || ch == '\n') && !isQuote) {

                if ((key == L"" && value == L"" && objList->size() == 0 && arrValueList->size() == 0)
                    || (key == L"" && value == L"" && !isArrEnd)) continue;

                addArray:   std::wstring inputKey = key == L"" ? L"" : key.replace(0, 1, L"");
                inputKey = inputKey == L"" ? L"" : inputKey.replace(inputKey.find_last_of(L"\""), 1, L"");

                JsonKey j_key;
                void* j_value;
                JsonType insertType = JsonType::Null;
                j_key._key = inputKey;

                if (value == L"true" || value == L"false") {                  //Bool
                    bool* temp=new bool(isBool(value));
                    j_value = temp;
                    j_key._type = JsonType::Bool;
                }
                else if (value.find(L"\"") == 0 && value.find_last_of(L"\"") == value.size() - 1) { //String
                    size_t n = value.size();
                    int size = sizeof(wchar_t)*n;
                    wchar_t* temp=new wchar_t[n+1];
                    memset(temp,0,size);
                    memcpy(temp, value.c_str(), size);
                    temp[n]=L'\0';
                    j_value=temp;
                    j_key._type = JsonType::String;
                }
                else if (isNumber(value)) {                                          //Number
                    double *result = new double(0);
                    std::wstringstream ss(value);
                    ss >> *result;
                    j_value=result;
                    j_key._type = JsonType::Number;
                }
                else if (arrValueList->size() != 0 && objList->size() != 0) {//Array Add or Object Add
                    if (!isObjEnd&&!isQuote&&ch!=L']') {
                        arrKey.push(inputKey);
                        key = L"";
                        isKey = true;
                        continue;
                    }
                    void* _val = curType == JsonType::Object ? (void*)objList->back().second : (void*)arrValueList->back().second;
                    size_t _si = curType == JsonType::Object ? objList->back().second->GetContent().size() : arrValueList->back().second->size();
                    j_key._key = arrKey.top();
                    j_key._type = curType;
                    j_value = _val;
                    if (curType == JsonType::Object) objList->pop_back();
                    else arrValueList->pop_back();
                    //接下来确认是放到obj 还是 arrvalue 通过cer栈顶元素判断
                    cer.pop();
                    JsonType upType = cer.top();
                    arrKey.pop();
                    if (upType == JsonType::Object)
                        objList->back().second->GetContent().insert(std::pair<JsonKey, void*>(j_key, j_value));
                    else
                        arrValueList->back().second->push_back((JsonSerialize*)j_value);
                    continue;
                }
                else if (objList->size() != 0) {//Object
                    if (!isObjEnd && isKey) {
                        arrKey.push(inputKey);
                        key = L"";
                        continue;
                    }
                    cer.pop();
                    j_key._key = arrKey.top();
                    arrKey.pop();
                    j_value=objList->back().second;
                    j_key._type = JsonType::Object;
                    objList->pop_back();
                    if (objList->size() != 0) {
                        objList->back().second->GetContent().insert(std::pair<JsonKey, void*>(j_key, j_value));
                        continue;
                    }
                }
                else if (arrValueList->size() != 0 && objList->size() == 0) {//Array
                    if (!isArrEnd) {
                        arrKey.push(inputKey);
                        key = L"";
                        isKey = true;
                        continue;
                    }
                    cer.pop();
                    j_key._key = arrKey.empty()?inputKey:arrKey.top();
                    if (!arrKey.empty()) arrKey.pop();
                    j_value = arrValueList->back().second;
                    j_key._type = JsonType::Array;
                    arrValueList->pop_back();
                    if (arrValueList->size() != 0) {
                        arrValueList->back().second->push_back((JsonSerialize*)j_value);
                        continue;
                    }
                }
                else if (value == L"") {                               //Null
                    j_value = new JNull();
                    j_key._type = JsonType::Null;
                }
                std::pair<JsonKey, void*> pair(j_key, j_value);
                if (!isObjEnd) {
                    objList->back().second->GetContent().insert(pair);
                }
                else {
                    content.insert(pair);
                }
                key = L"";
                value = L"";
                isKey = true;
                if (objListDeep == 1 && isObj) isObj = false;
            }
            else {
                if (isKey && isQuote) key += std::wstring(1, ch);
                else if (isKey && !isQuote) return false;
                else if (!isKey) value += std::wstring(1, ch);
            }
            if (objListDeep < 0 || arrDeep < 0) return false;

        }
        return true;
    }

    bool JsonSerialize::Load(const string& filePath) {
        try {
            if (!filePath.empty()) {
                this->_filePath = filePath;
            }
            OpenFile(this->_filePath);
            while (!_file.eof()) {
                wchar_t buffer[0x4000] = L"";
                ssize_t length = sizeof(buffer) / sizeof(wchar_t );
                _file.read(buffer, length);
                if (!Analysis(buffer, content, length)) {
                    CloseFile();
                    return false;
                }
            }
            CloseFile();
            freeAll();
            return objListDeep == 0 && arrDeep == 0;
        }
        catch (std::exception ex) {
            throw ex;
        }
    }

    map<JsonKey, void*>& JsonSerialize::GetContent() {
        return this->content;
    }

    void coutTab(int count) {
        while (count--) {
            std::cout << '\t';
        }
    }

    void JsonSerialize::printAll(int tab) {
        auto res = this->content;
        coutTab(tab);
//    std::cout.imbue(std::locale("chs"));
        std::wcout << "{" << std::endl;
        for (auto it = res.begin(); it != res.end(); it++) {
            JsonKey temp_key = it->first;
            if (temp_key._type == JsonType::Number) {
                JsonItem_t<JNumber> temp_value = JsonItem_t<JNumber>(it->second);
                coutTab(tab + 1);
                std::wcout << temp_key._key << " : " << *temp_value << std::endl;
            }
            else if (temp_key._type == JsonType::String) {
                JsonItem_t<JString> temp_value = JsonItem_t<JString>(it->second);
                // JsonItem<wchar_t*>* temp_value = (JsonItem<wchar_t*>*)(it->second._value);
                coutTab(tab + 1);
                std::wcout << temp_key._key << " : " << temp_value << "" << std::endl;
            }
            else if (temp_key._type == JsonType::Null) {
                JsonItem_t<JNull> temp_value = JsonItem_t<JNull>(it->second);
                coutTab(tab + 1);
                std::wcout << temp_key._key << " : " << "NULL" << std::endl;
            }
            else if (temp_key._type == JsonType::Bool) {
                JsonItem_t<JBool> temp_value = JsonItem_t<JBool>(it->second);
                coutTab(tab + 1);
                std::wcout << temp_key._key << " : " << (*temp_value ? "true" : "false") << std::endl;
            }
            else if (temp_key._type == JsonType::Object) {
                JsonItem_t<JObj> temp_value = JsonItem_t<JObj>(it->second);
                temp_value->printAll(tab + 1);
            }
            else if (temp_key._type == JsonType::Array) {
                JsonItem_t<JArray> temp_value = JsonItem_t<JArray>(it->second);
                coutTab(tab + 1);
                std::wcout << temp_key._key << ":[" << std::endl;
                for (auto item : *temp_value) {
                    JsonItem_t<JObj> tt_value = JsonItem_t<JObj>(item);
                    tt_value->printAll(tab + 2);
                }
                coutTab(tab + 1);
                std::wcout << "]" << std::endl;
            }
        }
        coutTab(tab);
        std::wcout << "}" << std::endl;
    }

    template<typename T>
    JsonItem_t<T> JsonSerialize::GetValueByKey(const std::wstring& key) const
    {
        // static_assert(!is_same<JsonItem_t<T>, JsonItem_t<JNull>>(), "type is JNull");
        JsonItem_t<T> res=nullptr;
        for(pair<JsonKey, void*> it : content){
            if(it.first._key==key){
                res=(JsonItem_t<T>)it.second;
                break;
            }
        }
        return res;
    }

    template<typename T>
    bool JsonSerialize::setValueByKey(const std::wstring& key, JsonItem_t<T> value){
        for(pair<JsonKey, void*> it : content){
            if(it.first._key==key){
                JsonKey t(it.first._type,it.first._key);
                void* pos=content.at(t);
                removeValue(it.first._type,pos);
                content.at(t)=value;
                return true;
            }
        }
        return false;
    }

    bool JsonSerialize::insert(const JsonType& type, const wstring& key, void* value){
        JsonKey j_key(type, key);
        auto ret = content.insert(pair<JsonKey,void*>(j_key,value));
        return ret.second;
    }


NAME_SPACE_END()