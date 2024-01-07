// #include "Util.h"
#include "base64.h"
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
    string str = Base64::encoding("123");
    cout<<str<<endl;
    string str1 = Base64::decoding(str);
    cout<<str1<<endl;


    // RBTree<int, int,less<int>> p;
    // // p.insert(1,1);
    // // p.insert(2,1);
    // // p.insert(3,1);
    // // p.insert(4,1);
    // // p.insert(5,1);
    // // p.insert(6,1);
    // // p.insert(7,1);
    // p.insert(pair<int, int>(1,1));
    // p.insert(pair<int, int>(2,1));
    // p.insert(pair<int, int>(3,1));
    // p.insert(pair<int, int>(4,1));
    // p.insert(pair<int, int>(5,1));
    // p.insert(pair<int, int>(6,1));
    // p.insert(pair<int, int>(7,1));
    // p.print();
    return 0;
}