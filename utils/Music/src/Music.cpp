#include "Music.h"
#include "Util.h"
#include <fstream>
using namespace std;
NAME_SPACE_START(myUtil)

bool MP3::analysis(){
    fstream file(filePath,ios::in|ios::binary);
    if(file.fail()) return false;
    try {
        file.read((char*)&head, sizeof(head));
        file.read((char*)&head, sizeof(head));
    } catch (...) {
        file.close();
        return false;
    }
    file.close();
    return false;
}

NAME_SPACE_END()