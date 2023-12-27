#include <iostream>
#include <string>
#include <cstring>
#include <memory>
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

int main(){
//    test();
    errno_t a = EOK;
    print(12);
    print(12.0);
    return 0;
}