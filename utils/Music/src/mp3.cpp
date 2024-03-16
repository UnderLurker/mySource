#include "mp3.h"

#include <fstream>
#include <fstream>
using namespace std;
NAME_SPACE_START(myUtil)

MusicStatus MP3::analysis(){
    try{
        fstream file(getFilePath(), ios::in | ios::binary);
        if (file.fail()) return ERROR_FILE_PATH;
        _id3v2.readData(file);
    }catch(...){
        return ERROR_UNKNOWN;
    }
    return SUCCESS;
}

NAME_SPACE_END()