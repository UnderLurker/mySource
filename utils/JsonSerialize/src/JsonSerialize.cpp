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
#include <codecvt>
#include "JsonSerialize.h"
#include "Singleton.h"

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

    wchar_t* str2lp(const wstring& input){
        wchar_t *res= nullptr;
        res=new wchar_t[input.size()+1];
        memset(res,0,sizeof(wchar_t)*input.size());
        memcpy(res,input.c_str(),sizeof(wchar_t)*input.size());
        res[input.size()]=L'\0';
        return res;
    }

    // wstring=>string
    std::string WString2String(const std::wstring &ws)
    {
        std::string strLocale = setlocale(LC_ALL, "");
        const wchar_t *wchSrc = ws.c_str();
        size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
        char *chDest = new char[nDestSize];
        memset(chDest, 0, nDestSize);
        wcstombs(chDest, wchSrc, nDestSize);
        std::string strResult = chDest;
        delete[] chDest;
        setlocale(LC_ALL, strLocale.c_str());
        return strResult;
    }

    // string => wstring
    std::wstring String2WString(const std::string &s)
    {
        std::string strLocale = setlocale(LC_ALL, "");
        const char *chSrc = s.c_str();
        size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
        wchar_t *wchDest = new wchar_t[nDestSize];
        wmemset(wchDest, 0, nDestSize);
        mbstowcs(wchDest, chSrc, nDestSize);
        std::wstring wstrResult = wchDest;
        delete[] wchDest;
        setlocale(LC_ALL, strLocale.c_str());
        return wstrResult;
    }

    void removeValue(const JsonType& type, void* lpValue){
        switch (type) {
            case Number:{
                delete (JsonItem_t<JNumber>)lpValue;
                break;
            }
            case String:{
                delete (JsonItem_t<JString>)lpValue;
                break;
            }
            case Array:{
                delete (JsonItem_t<JArray>)lpValue;
                break;
            }
            case Bool:{
                delete (JsonItem_t<JBool>)lpValue;
                break;
            }
            case Null:{
                delete (JsonItem_t<JNull>)lpValue;
                break;
            }
            case Object:{
                delete (JsonItem_t<JObj>)lpValue;
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

    JsonSerialize::JsonSerialize(const JsonSerialize &val) {
        for(auto& item : content){
            removeValue(item.first._type,item.second);
        }
        auto res = val.content;
        for (auto it = res.begin(); it != res.end(); it++) {
            JsonKey temp_key = it->first;
            if (temp_key._type == JsonType::Number) {
                auto *temp=new JsonNumber(*JsonItem_t<JNumber>(it->second));
                this->insert(temp_key._type,temp_key._key,temp);
            } else if (temp_key._type == JsonType::String) {
                auto *temp=new JsonString(*JsonItem_t<JString>(it->second));
                this->insert(temp_key._type,temp_key._key,temp);
            } else if (temp_key._type == JsonType::Null) {
                auto *temp=new JsonNull(*JsonItem_t<JNull>(it->second));
                this->insert(temp_key._type,temp_key._key,temp);
            } else if (temp_key._type == JsonType::Bool) {
                auto *temp=new JsonBool(*JsonItem_t<JBool>(it->second));
                this->insert(temp_key._type,temp_key._key,temp);
            } else if (temp_key._type == JsonType::Object) {
                auto *temp=new JsonObj(*JsonItem_t<JObj>(it->second));
                this->insert(temp_key._type,temp_key._key,temp);
            } else if (temp_key._type == JsonType::Array) {
                auto *temp = new JsonArray(*JsonItem_t<JArray>(it->second));
                this->insert(temp_key._type,temp_key._key,temp);
            }
        }
    }

    JsonSerialize::JsonSerialize(const string& filePath) {
        this->_filePath = filePath;
    }

    JsonSerialize::~JsonSerialize(){
        for(auto& item : content){
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
    std::vector<std::pair<bool,JsonArray*>> *arrValueList =
            new std::vector<std::pair<bool,JsonArray*>>(); //false直接放到对象中 true 放到objList中
    std::vector<std::pair<bool,JsonObj*>> *objList =
            new std::vector<std::pair<bool,JsonObj*>>();               //false直接放到对象中 true 放到arrValueList中

    void freeAll() {
        for (std::pair<bool, JsonArray*> item : *arrValueList)
            if (item.second!= nullptr) delete item.second;
        for (std::pair<bool, JsonObj*> item : *objList)
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

    bool Analysis(const wchar_t* buffer, std::map<JsonKey, JsonValue*>& content, const size_t length) {
        size_t charLength = wcslen(buffer) < length ? wcslen(buffer) : length;
        for (size_t i = 0; i < charLength; i++) {
            wchar_t ch = buffer[i];
            if (ch == '{' && !isQuote) {
                objListDeep++;
                curType = JsonType::Object;
                if (objListDeep >= 2) {
                    bool flag = !(cer.empty() || cer.top() == JsonType::Object);
                    objList->push_back(std::pair<bool, JsonObj*>(flag, new JsonObj()));
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
                arrValueList->push_back(std::pair<bool, JsonArray*>(flag,new JsonArray()));
                isArrEnd = false;
                //if (!isObjEnd) cer.push(JsonType::Object);
                cer.push(JsonType::Array);
            }
            else if (ch == ']' && !isQuote) {
                arrDeep--;
                curType = JsonType::Array;
                if (arrDeep == 0) isArrEnd = true;
                if(!key.empty()){
                    std::wstring inputKey = key == L"" ? L"" : key.replace(0, 1, L"");
                    inputKey = inputKey == L"" ? L"" : inputKey.replace(inputKey.find_last_of(L"\""), 1, L"");
                    arrKey.push(inputKey);
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

                addArray:
                std::wstring inputKey = key == L"" ? L"" : key.replace(0, 1, L"");
                inputKey = inputKey == L"" ? L"" : inputKey.replace(inputKey.find_last_of(L"\""), 1, L"");

                JsonKey j_key;
                JsonValue* j_value;
                JsonType insertType = JsonType::Null;
                j_key._key = inputKey;

                if (value == L"true" || value == L"false") {                  //Bool
                    JsonValue *temp=new JsonBool(isBool(value));
                    j_value = temp;
                    j_key._type = JsonType::Bool;
                }
                else if (value.find(L"\"") == 0 && value.find_last_of(L"\"") == value.size() - 1) { //String
                    JsonValue *temp=new JsonString(wstring(value.substr(1,value.size()-2)));
                    j_value=temp;
                    j_key._type = JsonType::String;
                }
                else if (isNumber(value)) {                                          //Number
                    double result = 0;
                    std::wstringstream ss(value);
                    ss >> result;
                    JsonValue *temp=new JsonNumber(result);
                    j_value=temp;
                    j_key._type = JsonType::Number;
                }
                else if (arrValueList->size() != 0 && objList->size() != 0) {//Array Add or Object Add
                    if (!isObjEnd&&!isQuote&&ch!=L']') {
                        arrKey.push(inputKey);
                        key = L"";
                        isKey = true;
                        continue;
                    }
                    if(curType==JsonType::Object)
                        j_value = new JsonObj(*objList->back().second);
                    else
                        j_value = new JsonArray(*arrValueList->back().second);
                    size_t _si = curType == JsonType::Object ? objList->back().second->GetContent().size() : arrValueList->back().second->size();
                    j_key._key = arrKey.top();
                    j_key._type = curType;
                    if (curType == JsonType::Object) objList->pop_back();
                    else arrValueList->pop_back();
                    //接下来确认是放到obj 还是 arrvalue 通过cer栈顶元素判断
                    cer.pop();
                    JsonType upType = cer.top();
                    arrKey.pop();
                    if (upType == JsonType::Object)
                        objList->back().second->GetContent().insert(std::pair<JsonKey, JsonValue*>(j_key, j_value));
                    else
                        arrValueList->back().second->push_back(*(JsonObj*)j_value);
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
                        objList->back().second->GetContent().insert(std::pair<JsonKey, JsonValue*>(j_key, j_value));
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
                    j_value = new JsonArray(*(arrValueList->back().second));
                    j_key._type = JsonType::Array;
                    arrValueList->pop_back();
                    if (arrValueList->size() != 0) {
                        arrValueList->back().second->push_back(*(JsonObj*)j_value);
                        continue;
                    }
                }
                else if (value == L"") {                               //Null
                    j_value = new JsonNull();
                    j_key._type = JsonType::Null;
                }
                std::pair<JsonKey, JsonValue*> pair(j_key, j_value);
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

    void coutTab(int count) {
        while (count--) {
            std::cout << '\t';
        }
    }

    void coutWTab(int count) {
        while (count--) {
            std::wcout << L'\t';
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
                auto temp_value = JsonItem_t<JNumber>(it->second);
                coutTab(tab + 1);
                std::wcout << temp_key._key << L" : " << (double)(*temp_value) << std::endl;
            }
            else if (temp_key._type == JsonType::String) {
                auto temp_value = JsonItem_t<JString>(it->second);
                // JsonItem<wchar_t*>* temp_value = (JsonItem<wchar_t*>*)(it->second._value);
                coutTab(tab + 1);
                std::wcout << temp_key._key << L" : \"" << (wstring)(*temp_value) << L"\"" << std::endl;
            }
            else if (temp_key._type == JsonType::Null) {
                auto temp_value = JsonItem_t<JNull>(it->second);
                coutTab(tab + 1);
                std::wcout << temp_key._key << L" : " << L"NULL" << std::endl;
            }
            else if (temp_key._type == JsonType::Bool) {
                auto temp_value = JsonItem_t<JBool>(it->second);
                coutTab(tab + 1);
                std::wcout << temp_key._key << L" : " << ((bool)(*temp_value) ? L"true" : L"false") << std::endl;
            }
            else if (temp_key._type == JsonType::Object) {
                auto temp_value = JsonItem_t<JObj>(it->second);
                temp_value->printAll(tab + 1);
            }
            else if (temp_key._type == JsonType::Array) {
                auto temp_value = JsonItem_t<JArray>(it->second);
                coutTab(tab + 1);
                std::wcout << temp_key._key << L":[" << std::endl;
                for (auto tt_it=temp_value->begin();tt_it!=temp_value->end();tt_it++) {
                    tt_it->printAll(tab + 2);
                }
                coutTab(tab + 1);
                std::wcout << "]" << std::endl;
            }
        }
        coutTab(tab);
        std::wcout << "}" << std::endl;
    }

    template<typename T>
    JsonItem_t_type<T> JsonSerialize::GetValueByKey(const std::wstring& key) const
    {
        // static_assert(!is_same<JsonItem_t<T>, JsonItem_t<JNull>>(), "type is JNull");
        JsonItem_t_type<T> res = JsonItem_t_type<T>();
        for(pair<JsonKey, JsonValue*> it : content){
            if(it.first._key==key){
                res=(JsonItem_t_type<T>)*(JsonItem_t<T>)it.second;
                break;
            }
        }
        return res;
    }

    bool JsonSerialize::setValueByKey(const std::wstring& key, JsonValue* value){
        for(pair<JsonKey, JsonValue*> it : content){
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

    bool JsonSerialize::insert(const JsonType& type, const wstring& key, JsonValue* value){
        JsonKey j_key(type, key);
        auto ret = content.insert(std::pair<JsonKey,JsonValue*>(j_key,value));
        return ret.second;
    }

    template<typename T>
    T* JsonSerialize::toRObject(const string& className) {
        Reflex* factory=Singleton<Reflex>::Instance();
        RObject* res = (RObject*)factory->createClass(className);
        for(const auto& item : content){
            string itemKey = WString2String(item.first._key.substr(1,item.first._key.size()-2));
            switch(item.first._type){
                case Number:{
                    res->set<double>(itemKey,(double)*JsonItem_t<JNumber>(item.second));
                    break;
                }
                case String:{
                    res->set<wstring>(itemKey,(wstring)*JsonItem_t<JString>(item.second));
                    break;
                }
//                case Array:{
//                    res->set<double>(itemKey,(double)*JsonItem_t<JNumber>(item.second));
//                    break;
//                }
                case Bool:{
                    res->set<bool>(itemKey,(bool)*JsonItem_t<JBool>(item.second));
                    break;
                }
//                case Null:{
//                    break;
//                }
//                case Object:{
//                    res->set<RObject*>(itemKey,(RObject*)(*JsonItem_t<JObj>(item.second)).toRObject<RObject*>());
//                    break;
//                }
                default:
                    break;
            }
        }
        return (T*)res;
    }

    bool JsonSerialize::operator==(const JsonSerialize& val){
        if(content.size()!=val.content.size()||_filePath!=val._filePath) return false;
        auto item1=content.begin();
        auto item2=val.content.begin();
        for(;item1!=content.end()&&item2!=val.content.end();item1++,item2++){
            if(item1->first!=item2->first) return false;
            switch (item1->first._type) {
                case Number:{
                    auto *temp_value1=(JsonItem_t<JNumber>)item1->second;
                    auto *temp_value2=(JsonItem_t<JNumber>)item2->second;
                    if(*temp_value1==*temp_value2) continue;
                    else return false;
                    break;
                }
                case String:{
                    auto *temp_value1=(JsonItem_t<JString>)item1->second;
                    auto *temp_value2=(JsonItem_t<JString>)item2->second;
                    if(*temp_value1==*temp_value2) continue;
                    else return false;
                    break;
                }
                case Null:{
                    auto *temp_value1=(JsonItem_t<JNull>)item1->second;
                    auto *temp_value2=(JsonItem_t<JNull>)item2->second;
                    if(*temp_value1==*temp_value2) continue;
                    else return false;
                    break;
                }
                case Object:{
                    auto *temp_value1=(JsonItem_t<JObj>)item1->second;
                    auto *temp_value2=(JsonItem_t<JObj>)item2->second;
                    if(*temp_value1==*temp_value2) continue;
                    else return false;
                    break;
                }
                case Array:{
                    auto *temp_value1=(JsonItem_t<JArray>)item1->second;
                    auto *temp_value2=(JsonItem_t<JArray>)item2->second;
                    if(*temp_value1==*temp_value2) continue;
                    else return false;
                    break;
                }
                case Bool:{
                    auto *temp_value1=(JsonItem_t<JBool>)item1->second;
                    auto *temp_value2=(JsonItem_t<JBool>)item2->second;
                    if(*temp_value1==*temp_value2) continue;
                    else return false;
                    break;
                }
                default:
                    break;
            }
        }
        return true;
    }

    wstring JsonSerialize::toString() {
        wstring res=L"{";
        for(auto item : content){
            res+=item.first._key+L":"+item.second->toString()+L",\n";
        }
        return res.substr(0,res.size()-2)+L"}";
    }

    JsonSerialize& JsonSerialize::operator=(const JsonSerialize& val){
        for(auto& item : content){
            removeValue(item.first._type,item.second);
        }
        for(auto& item : val.content) {
            JsonKey temp_key=item.first;
            if (temp_key._type == JsonType::Number) {
                auto *temp = new JsonNumber(*JsonItem_t<JNumber>(item.second));
                this->insert(temp_key._type, temp_key._key, temp);
            } else if (temp_key._type == JsonType::String) {
                auto *temp = new JsonString(*JsonItem_t<JString>(item.second));
                this->insert(temp_key._type, temp_key._key, temp);
            } else if (temp_key._type == JsonType::Null) {
                auto *temp = new JsonNull(*JsonItem_t<JNull>(item.second));
                this->insert(temp_key._type, temp_key._key, temp);
            } else if (temp_key._type == JsonType::Bool) {
                auto *temp = new JsonBool(*JsonItem_t<JBool>(item.second));
                this->insert(temp_key._type, temp_key._key, temp);
            } else if (temp_key._type == JsonType::Object) {
                auto *temp = new JsonObj(*JsonItem_t<JObj>(item.second));
                this->insert(temp_key._type, temp_key._key, temp);
            } else if (temp_key._type == JsonType::Array) {
                auto *temp = new JsonArray(*JsonItem_t<JArray>(item.second));
                this->insert(temp_key._type,temp_key._key,temp);
            }
        }
        return *this;
    }

    JsonArray::JsonArray(const JsonArray& obj){
        value.clear();
//        value.resize(obj.size());
        for(auto item = obj.cbegin();item!=obj.cend();item++){
            value.emplace_back(*item);
        }
    }

    bool JsonArray::operator==(const JsonArray& val){
        if(value.size()!=val.value.size()) return false;
        auto it1=value.begin();
        auto it2=val.value.begin();
        for(;it1!=value.end()&&it2!=val.value.end();it1++,it2++){
            if(*it1==*it2) continue;
            else
                return false;
        }
        return true;
    }

    JsonArray& JsonArray::operator=(const JsonArray& val){
        value.clear();
        for(auto item = val.cbegin();item!=val.cend();item++){
            value.emplace_back(*item);
        }
        return *this;
    }


    bool JsonSerialize::saveJson(const string& filePath){
        wofstream file(filePath,ios::out);
        if(!file.is_open()) return false;
        file<<toString();
        file.close();
        return true;
    }

NAME_SPACE_END()
