#include <iostream>
#include <string>
#include "Util.h"
#include "Matrix.h"
#include "logger.h"
#include "Singleton.h"
#include "Reflex.h"
using namespace std;
using namespace myUtil;

class A:public RObject{
public:
    void show(){
        cout<<"hello world"<<endl;
    }
    int add(int a,int b){
        return a+b;
    }
    //SINGLETON_DECLARE(A)
    int m_age;
    A():m_age(10){}
};

REGISTER_REFLEX(A)
REGISTER_REFLEX_FIELD(A, int, m_age)
REGISTER_REFLEX_METHOD(A, show)
REGISTER_REFLEX_METHOD_ARGS(A, add, int,int,int)

int main(){
    Reflex* factory=Singleton<Reflex>::Instance();
    A* a=(A*)factory->createClass("A");
    cout<<a->get<int>("m_age")<<endl;
    a->set<int>("m_age", 30);
    cout << a->get<int>("m_age") << endl;
    a->Call("show");
    int b = a->Call<int,int,int>("add",1,5);
    cout << b << endl;
    A* c=(A*)factory->createClass("A");
    cout<<c->get<int>("m_age")<<endl;
    c->set<int>("m_age", 40);
    cout << c->get<int>("m_age") << endl;
    c->Call("show");
    b = c->Call<int,int,int>("add",2,5);
    cout << b << endl;
    return 0;
}