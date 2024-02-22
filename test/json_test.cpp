#include <iostream>
#include <vector>
#include <string>
// #include "Matrix.h"
// #include "logger.h"
//#include "JsonSerialize.h"
#include "JsonParse.h"
//#include "Reflex.h"
using namespace std;
using namespace myUtil;

//class A :public RObject{
//public:
//    double type;
//    wstring request;
//    wstring program;
//};
//
//REGISTER_REFLEX(A)
//REGISTER_REFLEX_FIELD(A,double,type)
//REGISTER_REFLEX_FIELD(A,wstring,request)
//REGISTER_REFLEX_FIELD(A,wstring,program)

//void JsonSerializeTest()
//{
//    JsonSerialize json(R"(F:\C++\clion\mySource\sample\test.json)");
//    json.Load();
//    //    auto ret = json.GetValueByKey<JArray>(L"configurations");
//    //    auto res = json.GetValueByKey<JArray>(L"configurations")[0].GetValueByKey<JArray>(L"setupCommands")[0];
//    //    wcout<<res<<endl;
//    wcout << json.GetValueByKey<JString>(L"version") << endl;
//
//    //    JsonValue* ins=new JsonString(L"abcd");
//    //    json.setValueByKey(L"version", ins);
//    //    wcout<<json.GetValueByKey<JString>(L"version")<<endl;
//
//    //    JsonValue* number=new JsonNumber(20);
//    //    json.GetValueByKey<JArray>(L"configurations")[0].insert(Number,L"number",number);
//
//    //    JsonValue* array=new JsonArray(json.GetValueByKey<JArray>(L"configurations"));
//    //    json.insert(Array,L"array",array);
//
//    json.printAll();
//    //    wcout<<json.toString()<<endl;
//    //    json.saveJson(R"(F:\C++\clion\mySource\sample\test1.json)");
//
//    //    A* b = json.toRObject<A>("A");
//    //    wcout<<b->type<<L"\t"<<b->program<<L"\t"<<b->request<<endl;
//}

void JsonParseTest()
{
    JsonDocument doc("../sample/1.json");
}

int main() {
    JsonParseTest();
    return 0;
} 