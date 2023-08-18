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
    JsonSerialize json(R"(F:\C++\clion\mySource\sample\test.json)");
    json.Load();
//    auto ret = json.GetValueByKey<JArray>(L"configurations");
//    auto res = json.GetValueByKey<JArray>(L"configurations")[0].GetValueByKey<JArray>(L"setupCommands")[0];
////    wcout<<res<<endl;
//    wcout<<json.GetValueByKey<JString>(L"version")<<endl;

//    JsonValue* ins=new JsonString(L"abcd");
//    json.setValueByKey(L"version", ins);
//    wcout<<json.GetValueByKey<JString>(L"version")<<endl;

//    JsonValue* number=new JsonNumber(20);
//    json.GetValueByKey<JArray>(L"configurations")[0].insert(Number,L"number",number);

//    JsonValue* array=new JsonArray(json.GetValueByKey<JArray>(L"configurations"));
//    json.insert(Array,L"array",array);


    json.printAll();
//    wcout<<json.toString()<<endl;
    json.saveJson(R"(F:\C++\clion\mySource\sample\test1.json)");
//    json.saveJson(R"(F:\C++\clion\mySource\sample\test1.json)");

    return 0;
} 