#include "Util.h"
#include <iostream>
#include <sstream>
#include <atomic>
#include <exception>
#include <fstream>
#include <ios>
#include <string>
#include <utility>
#include <vector>
#include <stack>
#include <locale>
#include "JsonSerialize.h"

NAME_SPACE_START(Json)


// template<typename T>
// T& JHolderObject<T>::GetValue(){
//     return value;
// }


// template<typename ValueType>
// ValueType* JObject<ValueType>::Cast(){
//     JHolderObject<ValueType>* temp=dynamic_cast<JHolderObject<ValueType>*>(_value);
//     return temp?&temp->GetValue():NULL;
// }

// template<typename ValueType>
// ValueType& JObject<ValueType>::RefCast(){
//     return (dynamic_cast<JHolderObject<ValueType>&>(_value)).GetValue();
// }

// template<typename T>
// JsonItem<T>::JsonItem(const JObject<T>& value)
//     :JsonItemBase<T>(){
//     this->_value=value;
// }

// //获取值
// template<typename T>
// T& JsonItem<T>::GetItemRefValue(){
//     return this->_value.RefCast();
// }

// template<typename T>
// T* JsonItem<T>::GetItemLpValue(){
//     return this->_value.Cast();
// }

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

bool JsonKey::operator<(const JsonKey& j_key) const {
    return false;
}

JsonSerialize::JsonSerialize(const std::string filePath) {
    this->_filePath = filePath;
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
std::vector<std::pair<bool,JsonItem<std::vector<JsonValue>>*>> arrValueList; //false直接放到对象中 true 放到objList中
std::vector<std::pair<bool,JsonItem<JsonSerialize>*>> objList;               //false直接放到对象中 true 放到arrValueList中

void freeAll() {
    for (std::pair<bool, JsonItem<std::vector<JsonValue>>*> item : arrValueList)
        if (item.second) delete item.second;
    for (std::pair<bool, JsonItem<JsonSerialize>*> item : objList)
        if (item.second) delete item.second;
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

bool Analysis(const wchar_t* buffer, std::vector<std::pair<JsonKey, JsonValue>>& content, const size_t length) {
    size_t charLength = wcslen(buffer) < length ? wcslen(buffer) : length;
    for (size_t i = 0; i < charLength; i++) {
        wchar_t ch = buffer[i];
        if (ch == '{' && !isQuote) {
            objListDeep++;
            curType = JsonType::Object;
            if (objListDeep >= 2) {
                bool flag = (cer.empty() || cer.top() == JsonType::Object) ? false : true;
                objList.push_back(std::pair<bool, JsonItem<JsonSerialize>*>(flag, new JsonItem<JsonSerialize>()));
                isObj = true;
                isObjEnd = false;
                //if (!isArrEnd) cer.push(JsonType::Array);
                cer.push(JsonType::Object);
                if (isKey) arrKey.push(L"");
            }
        }
        else if (ch == '}' && !isQuote) {
            if (objListDeep == 1 && isFirst == true) isFirst = false;
            else if (objListDeep == 1 && isFirst == false) return false;
            objListDeep--;
            isObjEnd = true;
            curType = JsonType::Object;
            if (objListDeep != 0)
                goto addArray;
        }
        else if (ch == '[' && !isQuote) {
            bool flag = (cer.empty() || cer.top() == JsonType::Object) ? false : true;
            curType = JsonType::Array;
            arrDeep++;
            arrValueList.push_back(std::pair<bool, JsonItem<std::vector<JsonValue>>*>(flag,new JsonItem<std::vector<JsonValue>>()));
            isArrEnd = false;
            //if (!isObjEnd) cer.push(JsonType::Object);
            cer.push(JsonType::Array);
        }
        else if (ch == ']' && !isQuote) {
            arrDeep--;
            curType = JsonType::Array;
            if (arrDeep == 0) isArrEnd = true;
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

            if ((key == L"" && value == L"" && objList.size() == 0 && arrValueList.size() == 0)
                || (key == L"" && value == L"" && !isArrEnd)) continue;

addArray:   std::wstring inputKey = key == L"" ? L"" : key.replace(0, 1, L"");
            inputKey = inputKey == L"" ? L"" : inputKey.replace(inputKey.find_last_of(L"\""), 1, L"");

            JsonKey j_key;
            JsonValue j_value;
            JsonType insertType = JsonType::Null;
            j_key._key = inputKey;

            if (value == L"true" || value == L"false") {                  //Bool
                JsonItem<bool> *objList=new JsonItem<bool>(isBool(value));
                size_t size = sizeof(objList);
                j_value._value = objList;
                j_value._size = 1;
                j_key._type = JsonType::Bool;
            }
            else if (value.find(L"\"") == 0 && value.find_last_of(L"\"") == value.size() - 1) { //String
                size_t n = value.size();
                std::wstring* temp = new std::wstring(value.begin(), value.end());
                JsonItem<std::wstring*> *objList=new JsonItem<std::wstring*>(temp);
                j_value._value = objList;
                j_value._size = n;
                j_key._type = JsonType::String;
            }
            else if (isNumber(value)) {                                          //Number
                double result = 0;
                std::wstringstream ss(value);
                ss >> result;
                JsonItem<double> *objList=new JsonItem<double>(result);
                size_t size = sizeof(objList);
                j_value._value = objList;
                j_value._size = 1;
                j_key._type = JsonType::Number;
            }
            else if (arrValueList.size() != 0 && objList.size() != 0) {//Array Add or Object Add
                if (!isObjEnd) {
                    arrKey.push(inputKey);
                    key = L"";
                    isKey = true;
                    continue;
                }
                void* _val = curType == JsonType::Object ? (void*)objList.back().second : (void*)arrValueList.back().second;
                size_t _si = curType == JsonType::Object ?
                    objList.back().second->GetItemRefValue().GetContent().size() :
                    arrValueList.back().second->GetItemLpValue()->size();
                j_key._key = arrKey.top();
                j_value._value = _val;
                j_value._size = _si;
                j_key._type = curType;
                if (curType == JsonType::Object) objList.pop_back();
                else arrValueList.pop_back();
                //接下来确认是放到obj 还是 arrvalue 通过cer栈顶元素判断
                cer.pop();
                JsonType upType = cer.top();
                arrKey.pop();
                if (upType == JsonType::Object)
                    objList.back().second->GetItemLpValue()->GetContent().push_back(std::pair<JsonKey, JsonValue>(j_key, j_value));
                else 
                    arrValueList.back().second->GetItemLpValue()->push_back(j_value);
                continue;
            }
            else if (objList.size() != 0) {//Object
                if (!isObjEnd && isKey) {
                    arrKey.push(inputKey);
                    key = L"";
                    continue;
                }
                cer.pop();
                j_key._key = arrKey.top();
                arrKey.pop();
                j_value._value = objList.back().second;
                j_value._size = objList.back().second->GetItemRefValue().GetContent().size();
                j_key._type = JsonType::Object;
                objList.pop_back();
                if (objList.size() != 0) {
                    objList.back().second->GetItemLpValue()->GetContent().push_back(std::pair<JsonKey, JsonValue>(j_key, j_value));
                    continue;
                }
            }
            else if (arrValueList.size() != 0 && objList.size() == 0) {//Array
                if (!isArrEnd) {
                    arrKey.push(inputKey);
                    key = L"";
                    isKey = true;
                    continue;
                }
                cer.pop();
                j_key._key = arrKey.empty()?inputKey:arrKey.top();
                if (!arrKey.empty()) arrKey.pop();
                j_value._value = arrValueList.back().second;
                j_value._size = arrValueList.back().second->GetItemLpValue()->size();
                j_key._type = JsonType::Array;
                arrValueList.pop_back();
                if (arrValueList.size() != 0) {
                    arrValueList.back().second->GetItemLpValue()->push_back(j_value);
                    continue;
                }
            }
            else if (value == L"") {                               //Null
                JsonItem<JNull>* objList = new JsonItem<JNull>();
                size_t size = sizeof(objList);
                j_value._value = objList;
                j_value._size = 0;
                j_key._type = JsonType::Null;
            }
            std::pair<JsonKey, JsonValue> pair(j_key, j_value);
            if (!isObjEnd) {
                objList.back().second->GetItemRefValue().GetContent().push_back(pair);
            }
            else content.push_back(pair);
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

bool JsonSerialize::Load(const string filePath) {
    try {
        if (filePath != "") {
            this->_filePath = filePath;
        }
        OpenFile(this->_filePath);
        while (!_file.eof()) {
            wchar_t buffer[0x4000] = L"";
            size_t length = sizeof(buffer) / sizeof(char);
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

std::vector<std::pair<JsonKey, JsonValue>>& JsonSerialize::GetContent() {
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
    std::cout.imbue(std::locale("chs"));
    std::cout << "{" << std::endl;
    for (auto it = res.begin(); it != res.end(); it++) {
        JsonKey temp_key = it->first;
        if (temp_key._type == JsonType::Number) {
            JsonItem<double>* temp_value = (JsonItem<double>*)(it->second._value);
            coutTab(tab + 1);
            std::wcout << temp_key._key << " : " << temp_value->GetItemRefValue() << std::endl;
        }
        else if (temp_key._type == JsonType::String) {
            JsonItem<std::wstring*>* temp_value = (JsonItem<std::wstring*>*)(it->second._value);
            coutTab(tab + 1);
            std::wcout << temp_key._key << " : " << *temp_value->GetItemRefValue() << "" << std::endl;
        }
        else if (temp_key._type == JsonType::Null) {
            JsonItem<JNull>* temp_value = (JsonItem<JNull>*)(it->second._value);
            coutTab(tab + 1);
            std::wcout << temp_key._key << " : " << "NULL" << std::endl;
        }
        else if (temp_key._type == JsonType::Bool) {
            JsonItem<bool>* temp_value = (JsonItem<bool>*)(it->second._value);
            coutTab(tab + 1);
            std::wcout << temp_key._key << " : " << (temp_value->GetItemRefValue() ? "true" : "false") << std::endl;
        }
        else if (temp_key._type == JsonType::Object) {
            JsonItem<JsonSerialize>* temp_value = (JsonItem<JsonSerialize>*)(it->second._value);
            temp_value->GetItemRefValue().printAll(tab + 1);
        }
        else if (temp_key._type == JsonType::Array) {
            JsonItem<std::vector<JsonValue>>* temp_value =
                (JsonItem<std::vector<JsonValue>>*)(it->second._value);
            coutTab(tab + 1);
            std::wcout << temp_key._key << ":[" << std::endl;
            for (auto item : temp_value->GetItemRefValue()) {
                JsonItem<JsonSerialize>* tt_value = (JsonItem<JsonSerialize>*)(item._value);
                tt_value->GetItemLpValue()->printAll(tab + 2);
            }
            coutTab(tab + 1);
            std::cout << "]" << std::endl;
        }
    }
    coutTab(tab);
    std::cout << "}" << std::endl;
}


NAME_SPACE_END()