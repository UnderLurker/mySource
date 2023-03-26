#include <iostream>
#include <vector>
// #include "Matrix.h"
// #include "logger.h"
#include "JsonSerialize.h"
// #include "Reflex.h"
using namespace std;


int main() {
    /*JsonSerialize json("F:\\C++\\mySource\\example\\test.json");
    json.Load();
    json.printAll();*/
    wifstream file;
    locale china("zh_CN.UTF-8");
    file.open("F:\\C++\\mySource\\.vscode\\launch.json");
    file.imbue(china);
    wstring str;
    while (!file.eof()) {
        file >> str;
        wcout << str << endl;
    }
    file.close();
    //cout << (json.GetValueByKey<char*>("program"))->GetItemRefValue() << endl;
    return 0;
} 