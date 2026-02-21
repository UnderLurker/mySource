#include "mp3.h"
#include "m4a.h"
#include <fstream>
using namespace std;
using namespace myUtil;

int main(){
//    MP3 a("../../../Sounds/1.mp3");
    M4a a("./Sounds/3.m4a");
    std::fstream output("./save.txt", std::ios::out);
    output << a.PrintTree(0, "| ").str();
    return 0;
}