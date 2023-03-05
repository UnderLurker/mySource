#include "JsonSerialize.h"
#include "Util.h"
#include <atomic>
#include <exception>
#include <fstream>
#include <ios>
#include <string>
#include <vector>

NAME_SPACE_START(Json)

template<typename T>
JBaseObject* JHolderObject<T>::clone(){
    return new JHolderObject<T>(this->value);
}

template<typename T>
T JHolderObject<T>::GetValue(){
    return value;
}

template<typename ValueType>
JObject<ValueType>::JObject(const ValueType& value){
    _value=new JHolderObject<ValueType>(value);
}

template<typename ValueType>
JObject<ValueType>::~JObject(){
    if(_value) delete _value;
}

template<typename ValueType>
JObject<ValueType>& JObject<ValueType>::operator=(const ValueType& value){
    if(_value) delete _value;
    _value=new JHolderObject<ValueType>(value);
    return *this;
}

template<typename ValueType>
ValueType* JObject<ValueType>::Cast(){
    JHolderObject<ValueType>* temp=dynamic_cast<JHolderObject<ValueType>*>(_value);
    return temp?&temp->GetValue():NULL;
}

template<typename ValueType>
ValueType& JObject<ValueType>::RefCast(){
    JHolderObject<ValueType>& temp=dynamic_cast<JHolderObject<ValueType>&>(&_value);
    return temp.GetValue();
}

template<typename T>
JsonItem<T>::JsonItem(const JsonType type,const string key,const JObject<T> value){
    this->_type=type;
    this->_value=value;
    this->_key=key;
}

//获取类型
template<typename T>
JsonType JsonItem<T>::GetItemType(){
    return this->_type;
}
//获取键值
template<typename T>
std::string JsonItem<T>::GetItemKey(){
    return this->_key;
}
//获取值
template<typename T>
JObject<T> JsonItem<T>::GetItemValue(){
    return this->_value;
}

std::ifstream _file;
void OpenFile(const std::string filePath){
    _file.open(filePath,std::ios::in);
    if(!_file.is_open()){
        throw std::exception("JsonSerialize读取的文件没有打开!!!");
    }
}
void CloseFile(){
    _file.close();
}

JsonSerialize::JsonSerialize(const std::string filePath){
    this->_filePath=filePath;
}

bool isFirst=true;
bool isQuote=false;
bool isKey=true;
int deep=0;
std::string key="";
std::string value="";

bool isBool(const std::string str){
    return str=="true";
}

bool Analysis(const char* buffer, std::vector<JsonItemBase>& content, const int length){
    for(int i=0;i<length;i++){
        char ch=buffer[length];
        if(ch=='{'&&!isQuote){
            deep++;
            if(deep==1&&isFirst==true) isFirst=false;
            else if(deep==1&&isFirst==false) return false;
        }
        else if(ch=='}'&&!isQuote) deep--;
        else if(ch=='"'&&!isQuote) isQuote=true;
        else if(ch=='"'&&isQuote) isQuote=false;
        else if(ch==':'&&!isQuote) isKey=false;
        else if((ch==',' || ch=='\n')&&!isQuote){
            if(value=="true"||value=="false"){
                JObject<bool> obj(isBool(value));
                JsonItem<bool> temp(JsonType::Bool,key,obj);
            }
            JsonItem<typename T>
            key="";
            value="";
            isKey=true;
        }
        else{
            if(isKey&&isQuote) key+=std::string(1,ch);
            else if(isKey&&!isQuote) return false;
            else if(!isKey) value+=std::string(1,ch);
        }
        if(deep<0) return false;

    }
    return true;
}

bool JsonSerialize::Load(const string filePath){
    try{
        if(filePath!=""){
            this->_filePath=filePath;
        }
        OpenFile(this->_filePath);
        while(!_file.eof()){
            char buffer[0x4000]="";
            int length=sizeof(buffer)/sizeof(char);
            _file.read(buffer, length);
            if(!Analysis(buffer, content, length)){
                CloseFile();
                return false;
            }
        }
        CloseFile();
        return true;
    }
    catch(std::exception ex){
        throw ex;
    }
}


NAME_SPACE_END()