#include <iostream>
#include <vector>
#include <string>
// #include "Matrix.h"
// #include "logger.h"
#include "JsonSerialize.h"
// #include "Reflex.h"
using namespace std;
using namespace myUtil;

int main() {
    JsonSerialize json(R"(D:\code\sample\test.json)");
    json.Load();
    wcout<<json.GetValueByKey<JArray>(L"configurations")->at(0)->GetValueByKey<JString>(L"name")<<endl;
    wcout<<json.GetValueByKey<JString>(L"version")<<endl;

    json.setValueByKey<JString>(L"version", str2lp(L"\"abcd\""));

    wcout<<json.GetValueByKey<JString>(L"version")<<endl;
    vector<JsonSerialize*> *v=new vector<JsonSerialize*>();
    json.insert(Number,L"abcd",new double(20));
    json.insert(Array,L"insertArray",v);
    json.printAll();

     //wchar_t test
//    wstring str=L"\"0.2.0\"";
//    int size=sizeof(wchar_t)*str.size();
//    wchar_t *temp = new wchar_t[size+1];
//    memset(temp,0,sizeof(wchar_t)*str.size());
//    memcpy(temp,str.c_str(),size);
//    temp[str.size()]=L'\0';
//    wcout<<temp<<L"|"<<endl;

    return 0;
} 