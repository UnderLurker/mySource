#pragma once
#include "Reflex.h"
#include "Util.h"
#include <cstdint>
#include <vadefs.h>
#include <vector>
#include <functional>
#ifndef _REFLEX_
#define _REFLEX_

NAME_SPACE_START(myUtil)
#include "Singleton.h"
#include <map>
#include <string>


class Field;
class RObject{
public:
    RObject(){}
    virtual ~RObject(){}
    std::string _className;

    template<typename T>
    T get(const std::string& fieldName);

    template<typename T>
    void set(const std::string& fieldName,const T& fieldValue);

    void Call(const std::string& methodName);
    template<typename T,typename... Args>
    T Call(const std::string& methodName, Args... args);
};

typedef RObject* (*construct)(void);

//使用方法,使用REGISTER_REFLEX注册，然后直接使用createClass即可
class Reflex{
    friend class myUtil::Singleton<Reflex>;
public:
    void ReflexRegister();
    RObject* createClass(const std::string& className);
    void RegisterClass(const std::string& className,construct constructMethod);

    void RegisterField(const std::string& className,const std::string& FieldName,const std::string& FieldType,const size_t& offset);
    void RegisterMethod(const std::string& className,const std::string& methodName,const uintptr_t& lpMethod);
    template<typename T>
    T getClassField(void* originPos, const std::string& className,const std::string& fieldName);
    template<typename T>
    void setClassField(void* originPos, const std::string& className,const std::string& fieldName,const T& fieldValue);
    uintptr_t getClassMethod(const std::string& className,const std::string& methodName);
private:
    Reflex();
    Reflex(const Reflex&);
    ~Reflex();
    Reflex& operator=(const Reflex&);
private:
    std::map<std::string, construct> m_classMap;
    std::map<std::string, std::map<std::string, Field>> m_fieldMap;
    std::map<std::string, std::map<std::string, uintptr_t>> m_methodMap;
};

//仅仅用来在reflex中注册使用
class RegisterClass{
public:
    RegisterClass(const std::string& className,construct constructMethod)
    {
        Reflex* factory = myUtil::Singleton<Reflex>::Instance();
        factory->RegisterClass(className, constructMethod);
    }
    RegisterClass(const std::string& className,const std::string& fieldName,const std::string& fieldType,const size_t& offset)
    {
        Reflex* factory = myUtil::Singleton<Reflex>::Instance();
        factory->RegisterField(className, fieldName, fieldType, offset);
    }
    RegisterClass(const std::string& className,const std::string& methodName,const uintptr_t& lpMethod)
    {
        Reflex* factory = myUtil::Singleton<Reflex>::Instance();
        factory->RegisterMethod(className, methodName, lpMethod);
    }
};

class Field{
private:
    std::string m_fieldType{""};
    //std::string m_fieldName{""};
    size_t m_offset{0};
public:
    Field(){}
    Field(const std::string& fieldType,const size_t& offset):
        m_fieldType(fieldType),m_offset(offset){}
    ~Field(){}
    
    inline std::string getFieldType(){return m_fieldType;}
    //inline std::string getFieldName(){return m_fieldName;}
    inline size_t getOffSet(){return m_offset;}

    inline void setFieldType(const std::string& type){ m_fieldType = type;}
    //inline void setFieldName(const std::string& name){ m_fieldName = name;}
    inline void setOffSet(const size_t& offset){ m_offset = offset;}
};

class Method{
public:
    Method(){}
    Method(const std::string& name,const std::uintptr_t& method):
        methodName(name),lpMethod(method){}
    ~Method(){}
    inline void setMethodName(const std::string& name) { methodName = name;}
    inline void setLpMethod(const uintptr_t& lp) { lpMethod = lp;}
    inline std::string getMethodName(){return methodName;}
    inline uintptr_t getLpMethod(){return lpMethod;}
private:
    std::string methodName{""};
    std::uintptr_t lpMethod{0};
};


#define REGISTER_REFLEX(className)                                      \
        RObject* construct##className()                                 \
        {                                                               \
            RObject* obj = new className();                             \
            obj->_className = #className;                               \
            return obj;                                                 \
        }                                                               \
        RegisterClass registerClass##className(#className,construct##className);

#define REGISTER_REFLEX_FIELD(className,fieldType,fieldName)            \
        RegisterClass registerClass##className##fieldType##fieldName(#className,#fieldName,#fieldType,OFFSET(className,fieldName));

#define REGISTER_REFLEX_METHOD(className,methodName)                    \
        std::function<void(className)> className##methodName = &className::methodName;\
        RegisterClass registerClass##className##method(#className,#methodName,(std::uintptr_t)&className##methodName);

#define REGISTER_REFLEX_METHOD_ARGS(className,methodName,returnType,...)                    \
        std::function<returnType(className,##__VA_ARGS__)> className##methodName = &className::methodName;\
        RegisterClass registerClass##className##method##returnType(#className,#methodName,(std::uintptr_t)&className##methodName);

NAME_SPACE_END()



#endif //!_REFLEX_