#include <iostream>
#include <string>
#include <vector>
// #include "JsonSerialize.h"
#include "JsonParse.h"
#include "Reflex.h"
using namespace std;
using namespace myUtil;

class C : public RObject {
public:
    string a;
    double b;
};

class B : public RObject {
public:
    string efg;
    double hig;
    C* uio;
};

class A : public RObject {
public:
    bool type;
    string request;
    string program;
    B* abc;
    C* bcd;
};

REGISTER_REFLEX(C)
REGISTER_REFLEX_FIELD(C, string, a)
REGISTER_REFLEX_FIELD(C, double, b)
REGISTER_REFLEX(B)
REGISTER_REFLEX_FIELD(B, string, efg)
REGISTER_REFLEX_FIELD(B, double, hig)
REGISTER_REFLEX_FIELD(B, C, uio)
REGISTER_REFLEX(A)
REGISTER_REFLEX_FIELD(A, bool, type)
REGISTER_REFLEX_FIELD(A, string, request)
REGISTER_REFLEX_FIELD(A, string, program)
REGISTER_REFLEX_FIELD(A, B, abc)
REGISTER_REFLEX_FIELD(A, C, bcd)

// void JsonSerializeTest()
//{
//     JsonSerialize json(R"(F:\C++\clion\mySource\sample\test.json)");
//     json.Load();
//     //    auto ret = json.GetValueByKey<JArray>(L"configurations");
//     //    auto res = json.GetValueByKey<JArray>(L"configurations")[0].GetValueByKey<JArray>(L"setupCommands")[0];
//     //    wcout<<res<<endl;
//     wcout << json.GetValueByKey<JString>(L"version") << endl;
//
//     //    JsonValue* ins=new JsonString(L"abcd");
//     //    json.setValueByKey(L"version", ins);
//     //    wcout<<json.GetValueByKey<JString>(L"version")<<endl;
//
//     //    JsonValue* number=new JsonNumber(20);
//     //    json.GetValueByKey<JArray>(L"configurations")[0].insert(Number,L"number",number);
//
//     //    JsonValue* array=new JsonArray(json.GetValueByKey<JArray>(L"configurations"));
//     //    json.insert(Array,L"array",array);
//
//     json.printAll();
//     //    wcout<<json.toString()<<endl;
//     //    json.saveJson(R"(F:\C++\clion\mySource\sample\test1.json)");
//
//     //    A* b = json.toRObject<A>("A");
//     //    wcout<<b->type<<L"\t"<<b->program<<L"\t"<<b->request<<endl;
// }

void JsonParseTest1() {
    JsonDocument doc("../../../sample/test.json");
    cout << doc.getNode().getStatus() << endl;
    auto node = doc.CreateNode(String);
    node->setKey("abc", 3);
    node->setValue("123", 3);
    doc.getNode().addChild(node);
    doc.getNode()[1].setKey("123", 3);
    doc.getNode().getString(cout);
}

void JsonParseTest2() {
    JsonDocument doc("../../../sample/test.json");
    cout << doc.getNode().getStatus() << endl;
    doc.save("3.json");
}

void JsonParseTest3() {
    JsonDocument doc("../../../sample/test.json");
    cout << doc.getNode().getStatus() << endl;
    auto b = doc.getNode()[0];
    b.getString(cout);
    auto a = doc.getNode()["configurations"][0]["name"];
    cout << a.toString() << endl;
    doc.getNode().getString(cout);
}

void JsonParseTest4() {
    JsonDocument doc("../../../sample/3.json");
    cout << doc.getNode().getStatus() << endl;
    auto a = (A*)doc.getNode().toRObject("A", "B", "C");
    cout << a->type << endl;
    cout << a->program << endl;
    cout << a->request << endl;
    cout << a->abc->efg << endl;
    cout << a->abc->hig << endl;
    cout << a->abc->uio->a << endl;
    cout << a->abc->uio->b << endl;
}

void JsonParseTest5() {
    JsonDocument doc("../../../sample/3.json");
    cout << doc.getNode().getStatus() << endl;
    auto a = (A*)doc.getNode().toRObject("A", {"B", "C"}, "C");
    cout << a->type << endl;
    cout << a->program << endl;
    cout << a->request << endl;
    cout << a->abc->efg << endl;
    cout << a->abc->hig << endl;
    cout << a->abc->uio->a << endl;
    cout << a->abc->uio->b << endl;
    cout << a->bcd->a << endl;
    cout << a->bcd->b << endl;
}
int main() {
    //    JsonParseTest1();
    //    JsonParseTest2();
    //    JsonParseTest3();
    JsonParseTest4();
    JsonParseTest5();
    return 0;
}