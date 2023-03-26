#pragma once
#include "Util.h"
#include "Singleton.h"
#include <cstdint>
#include <vadefs.h>
#include <vector>
#include <functional>
#include <map>
#include <string>
#ifndef _REFLEX_
#define _REFLEX_

NAME_SPACE_START(myUtil)

//因为编译器不支持类模板和实现分开写，所以放到一起了
class Field;
class Reflex;
class RObject {
public:
    RObject() {}
    virtual ~RObject() {}
    std::string _className;

    template<typename T>
    T get(const std::string& fieldName);

    template<typename T>
    void set(const std::string& fieldName, const T& fieldValue);

    void Call(const std::string& methodName);
    template<typename T, typename... Args>
    T Call(const std::string& methodName, Args... args);
};

typedef RObject* (*construct)(void);

//使用方法,使用REGISTER_REFLEX注册，然后直接使用createClass即可
class Reflex {
    SINGLETON_DECLARE(Reflex)
public:
    void ReflexRegister() {}
    RObject* createClass(const std::string& className);
    void ClassRegister(const std::string& className, construct constructMethod);

    void RegisterField(const std::string& className, const std::string& FieldName, const std::string& FieldType, const size_t& offset);
    void RegisterMethod(const std::string& className, const std::string& methodName, const uintptr_t& lpMethod);
    template<typename T>
    T getClassField(void* originPos, const std::string& className, const std::string& fieldName);
    template<typename T>
    void setClassField(void* originPos, const std::string& className, const std::string& fieldName, const T& fieldValue);

    uintptr_t getClassMethod(const std::string& className, const std::string& methodName);
private:
    std::map<std::string, construct> m_classMap;
    std::map<std::string, std::map<std::string, Field>> m_fieldMap;
    std::map<std::string, std::map<std::string, uintptr_t>> m_methodMap;
};

//仅仅用来在reflex中注册使用
class ClassRegister {
public:
    ClassRegister(const std::string& className, construct constructMethod)
    {
        Reflex* factory = myUtil::Singleton<Reflex>::Instance();
        factory->ClassRegister(className, constructMethod);
    }
    ClassRegister(const std::string& className, const std::string& fieldName, const std::string& fieldType, const size_t& offset)
    {
        Reflex* factory = myUtil::Singleton<Reflex>::Instance();
        factory->RegisterField(className, fieldName, fieldType, offset);
    }
    ClassRegister(const std::string& className, const std::string& methodName, const uintptr_t& lpMethod)
    {
        Reflex* factory = myUtil::Singleton<Reflex>::Instance();
        factory->RegisterMethod(className, methodName, lpMethod);
    }
};

class Field {
private:
    std::string m_fieldType{ "" };
    //std::string m_fieldName{""};
    size_t m_offset{ 0 };
public:
    Field() {}
    Field(const std::string& fieldType, const size_t& offset) :
        m_fieldType(fieldType), m_offset(offset) {}
    ~Field() {}

    inline std::string getFieldType() { return m_fieldType; }
    //inline std::string getFieldName(){return m_fieldName;}
    inline size_t getOffSet() { return m_offset; }

    inline void setFieldType(const std::string& type) { m_fieldType = type; }
    //inline void setFieldName(const std::string& name){ m_fieldName = name;}
    inline void setOffSet(const size_t& offset) { m_offset = offset; }
};

class Method {
public:
    Method() {}
    Method(const std::string& name, const std::uintptr_t& method) :
        methodName(name), lpMethod(method) {}
    ~Method() {}
    inline void setMethodName(const std::string& name) { methodName = name; }
    inline void setLpMethod(const uintptr_t& lp) { lpMethod = lp; }
    inline std::string getMethodName() { return methodName; }
    inline uintptr_t getLpMethod() { return lpMethod; }
private:
    std::string methodName{ "" };
    std::uintptr_t lpMethod{ 0 };
};

template<typename T>
T RObject::get(const std::string& fieldName)
{
    Reflex* factory = myUtil::Singleton<Reflex>::Instance();
    return factory->getClassField<T>(this, _className, fieldName);
}

template<typename T>
void RObject::set(const std::string& fieldName, const T& fieldValue)
{
    Reflex* factory = myUtil::Singleton<Reflex>::Instance();
    factory->setClassField(this, _className, fieldName, fieldValue);
}

void RObject::Call(const std::string& methodName)
{
    Reflex* factory = myUtil::Singleton<Reflex>::Instance();
    std::uintptr_t temp = factory->getClassMethod(_className, methodName);
    if (temp == 0) return;
    typedef std::function<void(decltype(this))> class_method;
    class_method* method = (class_method*)temp;
    (*method)(this);
}

template<typename T, typename... Args>
T RObject::Call(const std::string& methodName, Args... args)
{
    Reflex* factory = myUtil::Singleton<Reflex>::Instance();
    std::uintptr_t temp = factory->getClassMethod(_className, methodName);
    if (temp == 0) return T();
    typedef std::function<T(decltype(this), Args...)> class_method;
    class_method* method = (class_method*)temp;
    return (*method)(this, args...);
}

RObject* Reflex::createClass(const std::string& className)
{
    if (m_classMap.find(className) == m_classMap.end()) return nullptr;
    return m_classMap[className]();
}

void Reflex::ClassRegister(const std::string& className, construct constructMethod)
{
    if (m_classMap.find(className) != m_classMap.end()) {
        throw std::exception();
        return;
    }
    m_classMap.insert(std::pair<std::string, construct>(className, constructMethod));
    m_fieldMap[className] = std::map<std::string, Field>();
    m_methodMap[className] = std::map<std::string, uintptr_t>();
}

void Reflex::RegisterField(const std::string& className, const std::string& FieldName, const std::string& FieldType, const size_t& offset)
{
    m_fieldMap[className][FieldName] = Field(FieldType, offset);
}

void Reflex::RegisterMethod(const std::string& className, const std::string& methodName, const uintptr_t& lpMethod)
{
    m_methodMap[className][methodName] = lpMethod;
}

template<typename T>
T Reflex::getClassField(void* originPos, const std::string& className, const std::string& fieldName)
{
    if (m_fieldMap.find(className) == m_fieldMap.end()) {
        return T();
    }
    if (m_fieldMap[className].find(fieldName) == m_fieldMap[className].end()) {
        return T();
    }
    size_t offset = m_fieldMap[className][fieldName].getOffSet();
    return *(T*)((size_t)originPos + offset);
}

template<typename T>
void Reflex::setClassField(void* originPos, const std::string& className, const std::string& fieldName, const T& fieldValue)
{
    if (m_fieldMap.find(className) == m_fieldMap.end()) {
        return;
    }
    if (m_fieldMap[className].find(fieldName) == m_fieldMap[className].end()) {
        return;
    }
    size_t offset = m_fieldMap[className][fieldName].getOffSet();
    *(T*)((size_t)originPos + offset) = fieldValue;
}

uintptr_t Reflex::getClassMethod(const std::string& className, const std::string& methodName)
{
    if (m_fieldMap.find(className) == m_fieldMap.end()) {
        return 0;
    }
    if (m_methodMap[className].find(methodName) == m_methodMap[className].end()) {
        return 0;
    }
    return m_methodMap[className][methodName];
}

NAME_SPACE_END()


#define REGISTER_REFLEX(className)                                      \
        myUtil::RObject* construct##className()                                 \
        {                                                               \
            myUtil::RObject* obj = new className();                             \
            obj->_className = #className;                               \
            return obj;                                                 \
        }                                                               \
        myUtil::ClassRegister registerClass##className(#className,construct##className);

#define REGISTER_REFLEX_FIELD(className,fieldType,fieldName)            \
        myUtil::ClassRegister registerClass##className##fieldType##fieldName(#className,#fieldName,#fieldType,OFFSET(className,fieldName));

#define REGISTER_REFLEX_METHOD(className,methodName)                    \
        std::function<void(className)> className##methodName = &className::methodName;\
        myUtil::ClassRegister registerClass##className##methodName(#className,#methodName,(std::uintptr_t)&className##methodName);

#define REGISTER_REFLEX_METHOD_ARGS(className,methodName,returnType,...)                    \
        std::function<returnType(className,##__VA_ARGS__)> className##methodName = &className::methodName;\
        myUtil::ClassRegister registerClass##className##methodName##returnType(#className,#methodName,(std::uintptr_t)&className##methodName);

#endif //!_REFLEX;_