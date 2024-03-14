#include <string>

#include "QrEncode.h"
#include "bmp.h"
#include <iostream>
#include <sstream>
#include "Matrix.h"
using namespace std;
using namespace myUtil;
void print(Matrix<RGB> res){
    for(int i=0;i<res.row;i++){
        for(int j=0;j<res.col;j++){
            RGB d=res.getValue(i, j);
            cout<<dec<<"("<<(int)d.red<<","
                <<(int)d.green<<","
                <<(int)d.blue<<") ";
        }
        cout << endl;
    }
}
string string_to_hex(const string &str) // transfer string to hex-string
{
    string result ="";
    string tmp;
    stringstream ss;
    int n=str.size();
    for (int i = 0; i < str.size(); i++) {
        ss << hex << int(str[i]) << endl;
        ss >> tmp;
        result += tmp + '\n';
    }
    return result;
}
int main(int argc,char * argv[]){
    if(argc>3){
        cout<<"param error\n";
        return 0;
    }
    if(string(argv[1])=="--help"){
        cout<<"[content] [resultName] out resultName.bmp QRcode\n\n";
        return 0;
    }

    QREncode qr;
    Matrix<RGB> rgb=qr.encoding(argv[1]);
    // Matrix<RGB> rgb=qr.encoding("01234567890123456789012345678901234567890");
    // Matrix<RGB> rgb=qr.encoding("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    BMPData bmp(AmplifyMatrix<RGB>(rgb,AMPLIFY_LEVEL),rgb.col*AMPLIFY_LEVEL,rgb.row*AMPLIFY_LEVEL,true);
    bmp.GrayEncoder();
    string path = string(argv[2])+".bmp";
    bmp.write(path.c_str());
    return 0;
}
