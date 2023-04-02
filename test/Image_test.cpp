#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdlib.h>
#include "Image.h"
using namespace std;
using namespace myUtil;
int main(){
	string str="../img/Image/1.jpg";
	JPEGData data;
	data.readJPEG(str.c_str());
    return 0;
}