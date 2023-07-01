#include "Util.h"
#include <algorithm>
#include <iostream>
#include <locale>
#include <string.h>
using namespace std;
using namespace myUtil;

int main(){
    // wstring str1=L"";
    // auto res1 = Split(str1,'_');
    // wcout<<res1.size();
    // char str[]="abc中文";
    // wstring wstr=L"你好";
    // for(int i=0;i<strlen(str);i++){
    //     cout<<str[i]<<" ";
    // }
    // cout<<endl;

    string a=Base64::encoding("ABasfsdfGHID");
    cout<<a<<endl;
    cout<<Base64::decoding(a)<<endl;
    return 0;
}