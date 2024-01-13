// #include "Util.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <utility>

#include "RBTree.h"
#include "SingleLinkList.h"
#include "base64.h"
using namespace std;
using namespace myUtil;

void base64() {
    string str = Base64::encoding("123");
    cout<<str<<endl;
    string str1 = Base64::decoding(str);
    cout<<str1<<endl;
}

void rbtree() {
    RBTree<int, int,less<>> p;
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
}

void singleLinkList() {
    SingleLinkList<int> list;
    for(int i=0;i<4;i++){
        list.push(i + 1);
    }
    cout<<endl;
    cout<<"length:"<<list.length()<<endl;
    for(auto & it : list){
        cout << it.value()<<" ";
    }
    cout<<endl;

    SingleLinkList<int> a(list);
    cout<<"length:"<<a.length()<<endl;
    for(auto it = a.begin(); it != a.end(); it++){
        cout << it->value()<<" ";
    }

    vector<int> b = {1,2,3,4,5};
    SingleLinkList<int> c(&(*b.begin()), &(*b.end()));
    cout<<endl;
    cout<<"length:"<<c.length()<<endl;
    for(auto it = c.begin(); it != c.end(); it++){
        cout << it->value()<<" ";
    }

    cout<<endl;
    auto d = list;
    cout<<"length:"<<d.length()<<endl;
    for(auto it = d.begin(); it != d.end(); it++){
        cout << it->value()<<" ";
    }
}

int main(){

    singleLinkList();

    return 0;
}