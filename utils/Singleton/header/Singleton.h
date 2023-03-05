#pragma once
#include "Singleton.h"
#ifndef _SINGLETON_
#define _SINGLETON_
#include "Util.h"

NAME_SPACE_START(myUtil)


//单例模式
template<typename T>
class Singleton{
public:
    static T* Instance(){
        if(m_instance==nullptr){
            m_instance = new T();
        }
        return m_instance;
    }
private:
    Singleton();
    Singleton(const Singleton<T>&);
    ~Singleton();
    Singleton<T>& operator=(const Singleton<T>&);

private:
    static T* m_instance;
};

template<typename T>
T* Singleton<T>::m_instance=nullptr;

#define SINGLETON_DECLARE(className) friend class Singleton<className>;  \
        className();                        \
        className(const className&);        \
        ~className();                       \
        className& operator=(const className&);

NAME_SPACE_END()
#endif //!_SINGLETON_
