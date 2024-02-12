#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>

#include "compress.h"
#include "LZ77.h"
using namespace std;
using namespace myUtil;
int main(){
//    ZIP a("../sample/1.zip");
    LZ77 lib;
    uint32_t len = 0;
    uint8_t *output = nullptr;
    uint8_t input[] = { 1,2,3,3,2,3,1,2,3};
    cout << lib.encode(input, 9, output, len) <<endl;
    auto dic = lib.getDicQueue();
    for(const auto& item : dic){
        cout<<"x:"<<item._x<<" y:"<<item._y<< " ";
        for(int i=0;(item._valStart+i)<item._valEnd;i++){
            cout<<(int)*(item._valStart+i);
        }
        cout<<endl;
    }
    return 0;
}