#include <iostream>
#include "Matrix.h"
using namespace std;
int main(){
    auto a=Matrix<double>::Pascal(5);
    auto b=Matrix<double>::Magic(5);
    auto c=a*b;
    cout<<c<<endl;
    return 0;
}