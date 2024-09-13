#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <functional>
#include <map>
using namespace std;

int length = 10;

class MyClass {
public:
    MyClass(){
        curCount = MyClass::count++;
        cout<<"construct MyClass:"<<curCount<<endl;
    }
    ~MyClass(){
        cout<<"deconstruct MyClass:"<<curCount<<endl;
    }
    static int count;
    int curCount{0};
};

int MyClass::count = 0;

void allocator1(MyClass* array){
    for(int i=0;i<length;i++){
        cout<<"allocator:"<<(array+i)->curCount<<endl;
        (array+i)->curCount++;
    }
}

void test(){
    unique_ptr<MyClass[]> abc(new MyClass[length]{MyClass()});
    allocator1(abc.get());
    for(int i=0;i<length;i++){
        cout<<"test:"<<(abc.get()+i)->curCount<<endl;
    }
}

void print(int a){
    cout<<a<<endl;
}

void print(double a){
    cout<<a<<endl;
}

template<class T, T val>
struct one_constant {
    constexpr static T value = val;
    explicit operator T() { return val; }
    const T& operator()() { return val; }
};

using false_constant = one_constant<bool, false>;
using true_constant = one_constant<bool, true>;

template<class T1, class T2>
struct _is_same : public false_constant {};

template<class T1>
struct _is_same<T1, T1> : public true_constant {};

template<class X, class Y>
inline constexpr bool is_same_my = _is_same<X, Y>::value;

int main(){
//    test();
    cout << _is_same<int, float>::value << endl;
    cout << _is_same<int, int>::value << endl;
    cout << is_same_my<int, float> << endl;
    cout << is_same_my<int, int> << endl;
    auto a = _is_same<int, int>();
    cout << bool(a) << endl;
    return 0;
}