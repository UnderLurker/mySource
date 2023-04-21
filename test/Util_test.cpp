#include "Util.h"
#include <iostream>
using namespace std;
using namespace myUtil;

int main(){
    wstring str1=L"";
    auto res1 = Split(str1,'_');
    wcout<<res1.size();
    return 0;
}