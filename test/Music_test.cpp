#include "Music.h"
#include <fstream>
#include <iostream>
using namespace std;
using namespace myUtil;

int main(){
    MP3 a("../Sounds/1.mp3");
    a.analysis();
    return 0;
}