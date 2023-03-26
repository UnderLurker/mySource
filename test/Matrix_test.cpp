#include <iostream>
#include "Matrix.h"
using namespace std;
int main(){
    auto a=Matrix<double>::Pascal(5);
    auto b=Matrix<double>::Magic(5);
    cout<<a.Pow(3)<<endl;
    return 0;
}