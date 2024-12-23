#include <exception>
#include <map>
#include <string>
#include <utility>

#include "Reflex.h"
#include "Singleton.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

template<typename T>
T RObject::get(const std::string& fieldName) {
    Reflex* factory = myUtil::Singleton<Reflex>::Instance();
    return factory->getClassField<T>(this, _className, fieldName);
}

template<typename T>
void RObject::set(const std::string& fieldName, const T& fieldValue) {
    Reflex* factory = myUtil::Singleton<Reflex>::Instance();
    factory->setClassField(this, _className, fieldName, fieldValue);
}

std::string RObject::getType(const std::string& fieldName) {
    Reflex* factory = myUtil::Singleton<Reflex>::Instance();
    return factory->getFieldType(_className, fieldName);
}

size_t RObject::getTypeSize(const std::string& fieldName) {
    Reflex* factory = myUtil::Singleton<Reflex>::Instance();
    return factory->getFieldSize(_className, fieldName);
}

void RObject::Call(const std::string& methodName) {
    Reflex* factory     = myUtil::Singleton<Reflex>::Instance();
    std::uintptr_t temp = factory->getClassMethod(_className, methodName);
    if (temp == 0) return;
    typedef std::function<void(decltype(this))> class_method;
    auto* method = (class_method*)temp;
    (*method)(this);
}

template<typename T, typename... Args>
T RObject::Call(const std::string& methodName, Args... args) {
    Reflex* factory     = myUtil::Singleton<Reflex>::Instance();
    std::uintptr_t temp = factory->getClassMethod(_className, methodName);
    if (temp == 0) return T();
    typedef std::function<T(decltype(this), Args...)> class_method;
    auto* method = (class_method*)temp;
    return (*method)(this, args...);
}

RObject* Reflex::createClass(const std::string& className) {
    if (m_classMap.find(className) == m_classMap.end()) return nullptr;
    return m_classMap[className]();
}

void Reflex::ClassRegister(const std::string& className, construct constructMethod) {
    if (m_classMap.find(className) != m_classMap.end()) {
        throw std::exception();
        return;
    }
    m_classMap.insert(std::pair<std::string, construct>(className, constructMethod));
    m_fieldMap[className]  = std::map<std::string, Field>();
    m_methodMap[className] = std::map<std::string, uintptr_t>();
}

void Reflex::RegisterField(const std::string& className,
                           const std::string& FieldName,
                           const std::string& FieldType,
                           const size_t& offset,
                           const size_t& size) {
    m_fieldMap[className][FieldName] = Field(FieldType, offset, size);
}

void Reflex::RegisterMethod(const std::string& className, const std::string& methodName, const uintptr_t& lpMethod) {
    m_methodMap[className][methodName] = lpMethod;
}

template<typename T>
T Reflex::getClassField(void* originPos, const std::string& className, const std::string& fieldName) {
    if (m_fieldMap.find(className) == m_fieldMap.end()) { return T(); }
    if (m_fieldMap[className].find(fieldName) == m_fieldMap[className].end()) { return T(); }
    size_t offset = m_fieldMap[className][fieldName].getOffSet();
    return *(T*)((size_t)originPos + offset);
}

template<typename T>
void Reflex::setClassField(void* originPos,
                           const std::string& className,
                           const std::string& fieldName,
                           const T& fieldValue) {
    if (m_fieldMap.find(className) == m_fieldMap.end()) { return; }
    if (m_fieldMap[className].find(fieldName) == m_fieldMap[className].end()) { return; }
    size_t offset                     = m_fieldMap[className][fieldName].getOffSet();
    *(T*)((size_t)originPos + offset) = fieldValue;
}

uintptr_t Reflex::getClassMethod(const std::string& className, const std::string& methodName) {
    if (m_fieldMap.find(className) == m_fieldMap.end()) { return 0; }
    if (m_methodMap[className].find(methodName) == m_methodMap[className].end()) { return 0; }
    return m_methodMap[className][methodName];
}

std::string Reflex::getFieldType(const std::string& className, const std::string& fieldName) {
    if (m_fieldMap.find(className) == m_fieldMap.end()) { return ""; }
    if (m_fieldMap[className].find(fieldName) == m_fieldMap[className].end()) { return ""; }
    return m_fieldMap[className][fieldName].getFieldType();
}

size_t Reflex::getFieldSize(const std::string& className, const std::string& fieldName) {
    if (m_fieldMap.find(className) == m_fieldMap.end()) { return 0; }
    if (m_fieldMap[className].find(fieldName) == m_fieldMap[className].end()) { return 0; }
    return m_fieldMap[className][fieldName].getSize();
}

NAME_SPACE_END()
