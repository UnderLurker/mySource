// #include "Util.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <utility>

#include "BTree.h"
#include "heap.h"
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

void heapTest()
{
    vector<int> input { 5, 3, 10, 2, 1, 0, 8, 4 };
    heap<int, less<>> a;// or heap<int, greater<>> a
    a.print();
    for (int i = 10; i >= -5; i--) {
        a.push_back(i);
    }
    a.pop_back();
    a.push_back(-20);
    a.print();
    cout << "heap.begin:";
    for (auto it = a.begin(); it != a.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    cout << "heap.: :";
    for (auto item : a) {
        cout << item << " ";
    }
    cout << endl;
    cout << "heap.rbegin:";
    for (auto it = a.rbegin(); it != a.rend(); it++) {
        cout << *it << " ";
    }
    cout << endl;
    a.erase(1);
    a.print("erase 1");
    a.erase(17);
    a.print("erase 17");
    a.erase(14);
    a.print("erase 14");
    a.erase(a.begin());
    a.print("erase begin");
    a.erase(a.end());
    a.print("erase end");
    a.erase(a.rbegin());
    a.print("erase rbegin");
    a.erase(a.rend());
    a.print("erase rend");
    a.pop_front();
    a.print("pop_front");
}

int main(){

//    singleLinkList();
//    heapTest();
    return 0;
}