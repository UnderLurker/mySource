#include "Util.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <string.h>
#include <utility>
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
    // map<int, int> p;

    RBTree<int, int,less<int>> p;
    // p.insert(1,1);
    // p.insert(2,1);
    // p.insert(3,1);
    // p.insert(4,1);
    // p.insert(5,1);
    // p.insert(6,1);
    // p.insert(7,1);
    p.insert(pair<int, int>(1,1));
    p.insert(pair<int, int>(2,1));
    p.insert(pair<int, int>(3,1));
    p.insert(pair<int, int>(4,1));
    p.insert(pair<int, int>(5,1));
    p.insert(pair<int, int>(6,1));
    p.insert(pair<int, int>(7,1));
    p.print();
    return 0;
}