//
// Created by 常笑男 on 2024/1/7.
//
#include "Ini.h"
#include "Util.h"
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

NAME_SPACE_START(myUtil)

IniValue::IniValue()
{

}

IniValue::IniValue(const bool val){
    m_value=val?"true":"false";
}

IniValue::IniValue(const int val){
    m_value=std::to_string(val);
}

IniValue::IniValue(const char* val){
    m_value=val;
}

IniValue::IniValue(const std::string val){
    m_value=val;
}

IniValue& IniValue::operator=(const bool& val){
    IniValue temp(val);
    m_value=temp.m_value;
    return *this;
}

IniValue& IniValue::operator=(const int& val){
    IniValue temp(val);
    m_value=temp.m_value;
    return *this;
}
IniValue& IniValue::operator=(const char* val){
    m_value=val;
    return *this;
}
IniValue& IniValue::operator=(const std::string& val){
    m_value=val;
    return *this;
}

IniValue::operator bool() const{
    return m_value=="true"?true:false;
}

IniValue::operator int() const{
    std::stringstream ss(m_value);
    int res=0;
    ss>>res;
    return res;
}

IniValue::operator std::string() const{
    return m_value;
}

IniHelper::IniHelper(){
    loadIniFile();
}
IniHelper::IniHelper(const std::string& filePath){
    m_filePath=filePath;
    loadIniFile();
}

void IniHelper::saveIniConfig(){
    std::fstream file;
    file.open(m_filePath,std::ios_base::out);
    if(file.fail()) return;
    for(auto it=m_map.begin();it!=m_map.end();it++){
        std::string sectionName="["+it->first+"]\n";
        file<<sectionName;
        auto keySection = it->second;
        for(auto key=keySection.begin();key!=keySection.end();key++){
            std::string keyValue=key->first;
            keyValue.append("=").append(key->second.m_value).append("\n");
            file<<keyValue;
        }
    }
    file.close();
}

void IniHelper::LogIniConfig() const{
    for(auto it=m_map.begin();it!=m_map.end();it++){
        std::string sectionName="["+it->first+"]\n";
        std::cout<<sectionName;
        auto keySection = it->second;
        for(auto key=keySection.begin();key!=keySection.end();key++){
            std::string keyValue=key->first;
            keyValue.append("=").append(key->second.m_value).append("\n");
            std::cout<<keyValue;
        }
    }
}

void IniHelper::setIniConfig(const std::string& sectionName,const std::string& keyName,const std::string& value){
    if(m_map.find(sectionName)==m_map.end()){
        std::map<std::string, IniValue> temp;
        temp[keyName]=value;
        m_map[sectionName]=temp;
    }
    else{
        m_map[sectionName][keyName]=value;
    }
}

IniValue IniHelper::getIniConfig(const std::string& sectionName,const std::string& keyName,const std::string& defValue) const{
    if(m_map.find(sectionName)==m_map.end()) return defValue;
    std::map<std::string, IniValue> mapping=m_map.at(sectionName);
    if(mapping.find(keyName)==mapping.end()) return defValue;
    return mapping[keyName];
}

bool IniHelper::removeIniConfig(const std::string& sectionName,const std::string& keyName){
    try {
        if(m_map.find(sectionName)==m_map.end()) return true;
        auto pos = m_map.at(sectionName).find(keyName);
        if(pos==m_map.at(sectionName).end()) return true;
        m_map.at(sectionName).erase(pos);
        return true;
    } catch (std::exception ex) {
        //std::cout<<ex.what()<<std::endl;
        return false;
    }
}

void IniHelper::loadIniFile(){
    std::fstream file;
    file.open(m_filePath,std::ios_base::in);
    if(file.fail()) return;
    std::string sectionName="",t="";
    while(std::getline(file,t)){
        trim(t);        //去除前后空格
        if(t=="\n"||t=="") continue;
        else if(t[0]=='['){
            sectionName = t.substr(1,t.size()-2);
            std::map<std::string, IniValue> p;
            m_map[sectionName]=p;
        }
        else{
            if(sectionName=="") continue;
            int left=0,right=0,equalPos=0;
            equalPos=t.find("=");
            if(equalPos==std::string::npos) continue;
            std::string keyName="",keyValue="";
            keyName=t.substr(0,equalPos);
            keyValue=t.substr(equalPos+1,t.size()-1-equalPos);
            trim(keyName);
            trim(keyValue);
            m_map[sectionName][keyName]=keyValue;
        }
    }
    file.close();
}

void IniHelper::trim(std::string& str){
    REMOVE_SPACE(str);
    std::reverse(str.begin(), str.end());
    REMOVE_SPACE(str);
    std::reverse(str.begin(), str.end());
}

NAME_SPACE_END()