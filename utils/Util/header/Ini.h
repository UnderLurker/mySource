//
// Created by 常笑男 on 2024/1/7.
//

#ifndef MAIN_INI_H
#define MAIN_INI_H

#include <string>
#include <map>
#include "Util.h"

NAME_SPACE_START(myUtil)

#define INI_FILE_PATH "config.ini"

class IniValue{
public:
    friend class IniHelper;
    IniValue();
    IniValue(const bool val);
    IniValue(const int val);
    IniValue(const char* val);
    IniValue(const std::string val);
    IniValue& operator=(const bool& val);
    IniValue& operator=(const int& val);
    IniValue& operator=(const char* val);
    IniValue& operator=(const std::string& val);
    operator bool() const;
    operator int() const;
    operator std::string() const;
private:
    std::string m_value;
};

class IniHelper{
private:
    std::map<std::string, std::map<std::string, IniValue>> m_map;
    std::string m_filePath{INI_FILE_PATH};
public:
    IniHelper();
    IniHelper(const std::string& filePath);
    void saveIniConfig();
    void LogIniConfig() const;
    void setIniConfig(const std::string& sectionName,const std::string& keyName,const std::string& value);
    IniValue getIniConfig(const std::string& sectionName,const std::string& keyName,const std::string& defValue) const;
    bool removeIniConfig(const std::string& sectionName,const std::string& keyName);
protected:
    void loadIniFile();
    void trim(std::string& str);
};

NAME_SPACE_END()
#endif // MAIN_INI_H
