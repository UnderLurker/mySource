#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>

#include "compress.h"
#include "LZ77.h"
using namespace std;
using namespace myUtil;

void LZ77Test()
{
    {
        LZ77<uint8_t> lib;
        uint32_t len = 0;
        uint8_t* output = nullptr;
        uint8_t input[] = { 1, 2, 3, 3, 2, 3, 1, 2, 3 };
        cout << lib.encode(input, 9, output, len) << endl;
        auto dic = lib.getDicQueue();
        for (const auto& item : dic) {
            cout << "x:" << item._x << " y:" << item._y << " ";
#ifdef LZ77_DEBUG
            uint32_t count = item._y == 0 ? 1 : item._y;
#else
            uint32_t count = item._y == 0 ? 1 : 0;
#endif
            for (int i = 0; i < count; i++) {
                cout << (int)*(item._valStart + i);
            }
            cout << endl;
        }
        LZ77<uint8_t> dlib(lib.getDicQueue());
        uint8_t* deOutput = nullptr;
        uint32_t deLen = 0;
        cout << dlib.decode(nullptr, 0, deOutput, deLen) << endl;
        for (uint32_t i = 0; i < deLen; i++) {
            cout << (int)*(deOutput + i) << " ";
        }
        cout << endl << "-------------------------------" << endl;
    }
    {
        LZ77<char> lib;
        char* input = "123323123";
        char* output = nullptr;
        uint32_t len = 0;
        cout << lib.encode(input, 9, output, len) << endl;
        auto dic = lib.getDicQueue();
        for (const auto& item : dic) {
            cout << "x:" << item._x << " y:" << item._y << " ";
#ifdef LZ77_DEBUG
            uint32_t count = item._y == 0 ? 1 : item._y;
#else
            uint32_t count = item._y == 0 ? 1 : 0;
#endif
            for (int i = 0; i < count; i++) {
                cout << *(item._valStart + i);
            }
            cout << endl;
        }
        LZ77<char> dlib(lib.getDicQueue());
        char* deOutput = nullptr;
        uint32_t deLen = 0;
        cout << dlib.decode(nullptr, 0, deOutput, deLen) << endl;
        for (uint32_t i = 0; i < deLen; i++) {
            cout << *(deOutput + i) << " ";
        }
        cout << endl << "-------------------------------" << endl;
    }

}

int main(){
//    ZIP a("../sample/1.zip");
    LZ77Test();
    return 0;
}