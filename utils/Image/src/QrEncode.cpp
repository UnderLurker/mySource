#include "QrEncode.h"

#include <algorithm>
#include <bitset>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <stack>
#include <string>
#include <vector>

#include "Matrix.h"
#include "Util.h"
#include "bmp.h"

NAME_SPACE_START(myUtil)

#define BACKGROUND_COLOR RGB_WHITE

void print(string& code){
    for(int i=0;i<code.size();i++){
        if(i%8==0) cout<<" ";
        cout<<code[i];
    }
    cout<<endl;
}

vector<function<bool(int,int)>> MaskFunctions={
    [](int row,int column)->int{
        return (row+column)%2==0;
    },
    [](int row,int column)->int{
        return row%2==0;
    },
    [](int row,int column)->int{
        return column%3==0;
    },
    [](int row,int column)->int{
        return (row+column)%3==0;
    },
    [](int row,int column)->int{
        return ((int)floor(row/2.0)+(int)floor(column/3.0))%2==0;
    },
    [](int row,int column)->int{
        return ((row*column)%2+(row*column)%3)==0;
    },
    [](int row,int column)->int{
        return ((row*column)%2+(row*column)%3)%2==0;
    },
    [](int row,int column)->int{
        return ((row+column)%2+(row*column)%3)%2==0;
    }
};

vector<Matrix<int>> getMaskList(const Matrix<int>& data){
    vector<Matrix<int>> res;
    res.resize(8);
    for(int i=0;i<MaskFunctions.size();i++){
        Matrix<int> temp(data);
        for(int r=0;r<temp.row;r++){
            for(int c=0;c<temp.col;c++){
                if((temp.getValue(r, c)==5||temp.getValue(r, c)==6)&&MaskFunctions[i](r,c)){
                    temp.setValue(r, c, 6-temp.getValue(r, c));
                }
                else if(temp.getValue(r, c)==5||temp.getValue(r, c)==6){
                    temp.setValue(r, c, temp.getValue(r, c)-5);
                }
            }
        }
        res[i]=temp;
    }
    return res;
}

vector<int> Next(string pattern){
	vector<int> next;
	next.push_back(0);	//next容器的首位必定为0
	for (int i = 1, j = 0; i < pattern.length(); i++)
	{
		while (j > 0 && pattern[j] != pattern[i])
		{ 
			j = next[j - 1];
		}
		if (pattern[i] == pattern[j])
		{
			j++; 
		}
		next.push_back(j);
	}
	return next;
}

int KMPCount(const string& target,const string& pattern){
    int res=0;
    auto next=Next(pattern);
    for (int i = 0, j = 0; i < target.length(); i++)
	{
		while (j > 0 && target[i] != pattern[j])
		{
			j = next[j - 1];
		}
		if (target[i] == pattern[j])
		{
			j++;
		}
		if (j == pattern.length())
		{
            res++;
			j = next[j - 1];
		}
	}
    return res;
}

// The first rule gives the QR code a penalty for each group of five or more same-colored modules in a row (or column).
int Rule1(const Matrix<int>& data){
    // row
    int sum=0;
    for(int i=0;i<data.row;i++){
        int ContinueCount=1;
        for(int j=1;j<data.col;j++){
            if(data.getValue(i, j)==data.getValue(i, j-1)) ContinueCount++;
            else if(ContinueCount>=5){
                sum+=ContinueCount-3;
                ContinueCount=1;
            }
            else{
                ContinueCount=1;
            }
        }
        if(ContinueCount>=5) sum+=ContinueCount-3;
    }
    // col
    for(int i=0;i<data.col;i++){
        int ContinueCount=1;
        for(int j=1;j<data.row;j++){
            if(data.getValue(j, i)==data.getValue(j-1, i)) ContinueCount++;
            else if(ContinueCount>=5){
                sum+=ContinueCount-3;
                ContinueCount=1;
            }
            else{
                ContinueCount=1;
            }
        }
        if(ContinueCount>=5) sum+=ContinueCount-3;
    }
    return sum;
}

// The second rule gives the QR code a penalty for each 2x2 area of same-colored modules in the matrix.
int Rule2(const Matrix<int>& data){
    int sum=0;
    for(int r=0;r<data.row-1;r++){
        for(int c=0;c<data.col-1;c++){
            if(data.getValue(r, c)==data.getValue(r, c+1)&&
               data.getValue(r, c+1)==data.getValue(r+1, c)&&
               data.getValue(r+1, c)==data.getValue(r+1, c+1)){
                sum+=3;
            }
        }
    }
    return sum;
}

// The third rule gives the QR code a large penalty if there are patterns that look similar to the finder patterns.
int Rule3(const Matrix<int>& data){
    int sum=0;
    //row
    for(int r=0;r<data.row;r++){
        string temp="";
        for(int c=0;c<data.col;c++){
            temp.append(string(1,'0'+data.getValue(r, c)));
        }
        sum+=40*KMPCount(temp, "10111010000");
        sum+=40*KMPCount(temp, "00001011101");
    }
    //col
    for(int c=0;c<data.col;c++){
        string temp="";
        for(int r=0;r<data.row;r++){
            temp.append(string(1,'0'+data.getValue(r, c)));
        }
        sum+=40*KMPCount(temp, "10111010000");
        sum+=40*KMPCount(temp, "00001011101");
    }
    return sum;
}

// The fourth rule gives the QR code a penalty if more than half of the modules are dark or light, with a larger penalty for a larger difference.
int Rule4(const Matrix<int>& data){
    int sum=0,darkCount=0,total=data.row*data.col;
    for(int r=0;r<data.row;r++){
        for(int c=0;c<data.col;c++){
            if(data.getValue(r, c)==1) darkCount++;
        }
    }
    int number=ceil((1.0*darkCount)/total);
    int bigTemp=number%10>5?(10-number%10):(5-number%10);
    int smallTemp=number%10>5?(number%10-5):number%10;
    int big=abs(number+bigTemp-50)/5,small=abs(number-smallTemp-50)/5;
    sum=min(big,small)*10;
    return sum;
}

int Evaluate(const Matrix<int>& data){
    int sum=0;
    sum+=Rule1(data);
    sum+=Rule2(data);
    sum+=Rule3(data);
    sum+=Rule4(data);
    return sum;
}

int* getErrorCurrentVersion(ErrorCorrectionLevel level,int maskIndex){
    
    int LevelBitSequences[]={1,0,3,2};
    int cur=LevelBitSequences[(int)level],curMaskIndex=maskIndex;
    int polynomial[11]={1,0,1,0,0,1,1,0,1,1,1};
    int maskPattern[]={1,0,1,0,1,0,0,0,0,0,1,0,0,1,0};
    int *code=new int[15];
    memset(code, 0, sizeof(int)*15);
    for(int i=1;i>=0;i--){
        code[i]=cur%2;
        cur>>=1;
    }
    for(int i=4;i>1;i--){
        code[i]=curMaskIndex%2;
        curMaskIndex>>=1;
    }

    int pos=0;
    while(pos<5){
        while(code[pos]==0) pos++;
        for(int i=pos;i<15;i++){
            int temp=polynomial[i-pos];
            if(i-pos>10) temp=0;
            code[i]=code[i]^temp;
        }
        while(code[pos]==0) pos++;
    }
    
    cur=LevelBitSequences[(int)level],curMaskIndex=maskIndex;
    for(int i=1;i>=0;i--){
        code[i]=cur%2;
        cur>>=1;
    }
    for(int i=4;i>1;i--){
        code[i]=curMaskIndex%2;
        curMaskIndex>>=1;
    }
    for(int i=0;i<15;i++){
        code[i]=code[i]^maskPattern[i];
    }
    return code;
}

//获取纠错码字
vector<int> getErrorCurrentWords(int* coefficient,int cofLen,int ErrorCurrentTableIndex){
    int polynomialLen=ErrorCurrentTable[ErrorCurrentTableIndex][1]+1;
    int* coeff=new int[cofLen + polynomialLen - 1];
    memset(coeff, 0, sizeof(int)*(cofLen + polynomialLen - 1));
    memcpy(coeff, coefficient, sizeof(int)*cofLen);
    for(int i=0;i<cofLen;i++){
        // 1.Multiply the Generator Polynomial by the Lead Term of the XOR result from the previous step
        // 2.XOR the result with the result from step 14b
        int exponent=AntiLogTable[coeff[i]];
        for(int j=0;j<max(cofLen-i,polynomialLen);j++){
            int cof=LogTable[(exponent+GeneratorPolynomialCoff[polynomialLen-8][j])%255];
            if(j>=polynomialLen) cof=0;
            coeff[i+j] = coeff[i+j] ^ cof;
        }
    }
    vector<int> res(polynomialLen-1,0);
    for(int i=0;i<polynomialLen-1;i++) res[i]=coeff[cofLen+i];
    delete [] coeff;
    return res;
}

Matrix<RGB> QREncode::encoding(const string &encodeData){
    this->encodeData=encodeData;
    if(!init()) return Matrix<RGB>();
    imgData=new Matrix<RGB>(getSideLen(),getSideLen(),BACKGROUND_COLOR);

    // char encoding
    string code=bitset<MODE_INDICATOR_BIT_LENGTH>(ModeIndicator[(int)type]).to_string();
    code.append(bitset<16>(encodeData.size()).to_string().substr(16-charCount));
    if(type==DataType::NumMode) code.append(NumModeEncoding());
    else if(type==DataType::AlpNumMode) code.append(AlpNumModeEncoding());
    else if(type==DataType::ByteMode) code.append(ByteModeEncoding());
    else if(type==DataType::KanjiMode) code.append(KanjiModeEncoding());
    fillEncodeData(code);
    // print(code);
    errorCurrentEncoding(code);
    // cout<<code<<endl;
    // print(code);
    Matrix<int> source = MatrixCode(code);
    imgData->setValByArray(source, vector<RGB>{RGB_BLACK}, 0, 0);
    return imgData==nullptr?Matrix<RGB>():*imgData;
}

Matrix<RGB> QREncode::encoding(const wstring& encodeWData){
    this->encodeWData=encodeWData;
    if(!init()) return Matrix<RGB>();
    imgData=new Matrix<RGB>(getSideLen(),getSideLen(),RGB_WHITE);

    return imgData==nullptr?Matrix<RGB>():*imgData;
}

//默认找最小的版本
bool QREncode::init(){
    int NumMode=0,AlpNumMode=0,ByteMode=0;
    for_each(encodeData.begin(), encodeData.end(), [&](char ch){
        if(ch<='9'&&ch>='0') NumMode++;
        else if((ch<='z'&&ch>='a')) ByteMode++;
        else if(ch<='Z'&&ch>='A') AlpNumMode++;
    });
    if(ByteMode!=0) type=DataType::ByteMode;
    else if(AlpNumMode!=0) type=DataType::AlpNumMode;
    else if(NumMode!=0) type=DataType::NumMode;
    else type=DataType::KanjiMode;
    int len=encodeData.size();
    if (len > 7089 && type == DataType::NumMode ||
        len > 4296 && type == DataType::AlpNumMode ||
        len > 2953 && type == DataType::ByteMode ||
        len > 1817 && type == DataType::KanjiMode)
        return false;
    for(int i=0;i<VERSION_COUNT;i++){
        for(int j=3;j>=0;j--){
            if(CharCapTable[i][j][(int)type]>len){
                version=i+1;
                level=(ErrorCorrectionLevel)j;
                if(version>=1&&version<=9) 
                    charCount=CharCountIndicator[0][(int)type];
                else if(version>=10&&version<=26)
                    charCount=CharCountIndicator[1][(int)type];
                else if(version>=27&&version<=40)
                    charCount=CharCountIndicator[2][(int)type];
                return true;
            }
        }
    }
    return false;
}

string QREncode::NumModeEncoding(){
    string res="";
    //分为3个一组，最后会出现2个一组或者一个一组的
    //都转为10进制，然后3位的转为10位二进制，两位转为7位，一位转为4位
    for (int i = 0; i < encodeData.size(); i += 3) {
        int t=0, bitCount=4;
        if (i < encodeData.size()) t += encodeData[i] - '0';
        if (i + 1 < encodeData.size()){
            t = t * 10 + encodeData[i + 1] - '0';
            bitCount=7;
        }
        if (i + 2 < encodeData.size()){
            t = t * 10 + encodeData[i + 2] - '0';
            bitCount=10;
        }
        if(bitCount==4) res.append(bitset<4>(t).to_string());
        else if(bitCount==7) res.append(bitset<7>(t).to_string());
        else if(bitCount==10) res.append(bitset<10>(t).to_string());
    }
    return res;
}
string QREncode::AlpNumModeEncoding(){
    string res="";
    //两位转11位二进制 一位转6位二进制
    for(int i=0;i<encodeData.size();i+=2){
        int t=0, bitCount=6;
        if(i<encodeData.size()) 
            t = AlpValMappingTable[encodeData[i]];
        if(i+1<encodeData.size()){
            t = t*45 + AlpValMappingTable[encodeData[i+1]];
            bitCount=11;
        }
        if(bitCount==6) res.append(bitset<6>(t).to_string());
        else if(bitCount==11) res.append(bitset<11>(t).to_string());
    }
    return res;
}
string QREncode::ByteModeEncoding(){
    // 1.Convert to ISO 8859-1 or UTF-8
    // 2.Split the String into 8-bit Bytes
    // 3.Convert Each Byte into Binary
    // 4.Next: Finish the Data Encoding Step
    string res="";
    for(int i=0;i<encodeData.size();i++){
        res.append(bitset<8>(encodeData[i]).to_string());
    }
    return res;
}
string QREncode::KanjiModeEncoding(){
    // 1.Only for Double-Byte Shift JIS Characters
    // 2.Convert to Bytes
    // 3.Encode the Bytes with Kanji Mode
    //     For characters whose bytes are in the range 0x8140 to 0x9FFC
    //      for example: 0x89D7 
    //     1. 0x89D7 - 0x8140 = 0x0897
    //     2. (0x08 * 0xC0) + 0x97 = (0x600) + 0x97 = 0x697
    //     3. 0x697 -> binary(13bit)
    //     For characters whose bytes are in the range 0xE040 to 0xEBBF
    //      for example: 0xE4AA 
    //     1. 0xE4AA - 0xC140 = 0x236A
    //     2. (0x23 * 0xC0) + 0x6A = (0x1A40) + 0x6A = 0x1AAA
    //     3. 0x1AAA -> binary(13bit)
    // 4.Put the 13-bit Binary Numbers Together
    // 5.Next: Finish the Data Encoding Step
    string res="";
    for(int i=0;i<encodeWData.size();i++){

    }
    return res;
}

void QREncode::fillEncodeData(string& code){
    // 1. Determine the Required Number of Bits for this QR Code
    // 2. Add a Terminator of 0s if Necessary
    // 3. Add More 0s to Make the Length a Multiple of 8
    // 4. Add Pad Bytes if the String is Still too Short
    //  11101100 00010001//两字节循环填入
    // code string bit length
    int maxLen=ErrorCurrentTable[(version-1)*4+(int)level][0]*8;
    int diff=maxLen-code.size();
    if(diff<0) return;
    // Add a Terminator
    if(diff>4) code.append("0000");
    else if(diff<=4) code.append(string(diff,'0'));
    // Add More 0s to Make the Length a Multiple of 8
    int eightDiff = 8-code.size()%8;
    code.append(string(eightDiff,'0'));
    // Add Pad Bytes
    if(maxLen-code.size()<8) return;
    int n=(maxLen-code.size())/8;
    for(int i=0;i<n;i++){
        if(i%2==0) code.append("11101100");
        else code.append("00010001");
    }
}

void QREncode::errorCurrentEncoding(string& code){
    // Step 1: Break Data Codewords into Blocks if Necessary
    // Step 2: Understand Polynomial Long Division
    // Step 3: Understand The Galois Field
    // Step 4: Understand Galois Field Arithmetic
    // Step 5: Generate Powers of 2 using Byte-Wise Modulo 100011101
    // Step 6: Understand Multiplication with Logs and Antilogs
    // Step 7: Understanding The Generator Polynomial
    // Step 8: Generating Error Correction Codewords
    // Step 9: Divide the Message Polynomial by the Generator Polynomial
    int row=(version-1)*4+(int)level,codePos=0;
    int groupCount = ErrorCurrentTable[row][4]==0?1:2;
    int*** group=new int**[groupCount];
    vector<vector<int>> currentWords;
    int maxCodeWordsLen=0,maxCurrentCodeWordsLen=0;

    for(int i=0;i<groupCount;i++){
        int blockCol=2,codeWordsCol=3;
        if(i==1){
            blockCol=4;
            codeWordsCol=5;
        }
        group[i]=new int*[ErrorCurrentTable[row][blockCol]];
        for(int blockOfGroup=0;blockOfGroup<ErrorCurrentTable[row][blockCol];blockOfGroup++){
            group[i][blockOfGroup]=new int[ErrorCurrentTable[row][codeWordsCol]];
            maxCodeWordsLen=max(maxCodeWordsLen,ErrorCurrentTable[row][codeWordsCol]);
            for(int blocks=0;blocks<ErrorCurrentTable[row][codeWordsCol]&&codePos<=code.size();blocks++){
                int t=0;
                for(int k=0;k<8;k++){
                    t<<=1;
                    t+=code[codePos+k]-'0';
                }
                group[i][blockOfGroup][blocks]=t;
                codePos+=8;
                // cout<<t<<" ";
            }
            // cout<<endl;
            vector<int> CurrentCodeWords = getErrorCurrentWords(group[i][blockOfGroup], ErrorCurrentTable[row][codeWordsCol],row);
            currentWords.push_back(CurrentCodeWords);
            maxCurrentCodeWordsLen=max(maxCurrentCodeWordsLen,(int)CurrentCodeWords.size());
        }
    }
    // cout<<endl;
    // for(auto items : currentWords){
    //     for(int item : items){
    //         cout<<item<<" ";
    //     }
    //     cout<<endl;
    // }
    // cout<<endl;

    //Structure Final Message
    code = "";
    //Interleave the Blocks
    for(int i=0;i<maxCodeWordsLen;i++){
        for(int j=0;j<groupCount;j++){
            int blockCol=2,codeWordsCol=3;
            if(j==1){
                blockCol=4;
                codeWordsCol=5;
            }
            for(int k=0;k<ErrorCurrentTable[row][blockCol];k++){
                int dataLen=ErrorCurrentTable[row][codeWordsCol];
                if(i>=dataLen) continue;
                code.append(bitset<8>(group[j][k][i]).to_string());
                // cout<<group[j][k][i]<<" ";
            }
        }
    }
    // cout<<endl<<endl;
    //Interleave the Error Correction Codewords
    for(int i=0;i<maxCurrentCodeWordsLen;i++){
        for(int j=0;j<currentWords.size();j++){
            if(i>=currentWords[j].size()) continue;
            // cout<<"("<<j<<","<<i<<") ";
            code.append(bitset<8>(currentWords[j][i]).to_string());
            // cout<<currentWords[j][i]<<" ";
        }
    }
    // cout<<endl;
    //Add Remainder Bits
    code.append(string(RemainderBits[version-1],'0'));
}

vector<vector<int>> FinderPatterns={
    {1,1,1,1,1,1,1},
    {1,2,2,2,2,2,1},
    {1,2,1,1,1,2,1},
    {1,2,1,1,1,2,1},
    {1,2,1,1,1,2,1},
    {1,2,2,2,2,2,1},
    {1,1,1,1,1,1,1}
};

vector<vector<int>> AlignmentPatterns={
    {1,1,1,1,1},
    {1,2,2,2,1},
    {1,2,1,2,1},
    {1,2,2,2,1},
    {1,1,1,1,1}
};

Matrix<int> QREncode::MatrixCode(const string& code){
    Matrix<int> res(getSideLen(),getSideLen(),-1);
    Matrix<int> finder(7,7,FinderPatterns),
                alignment(5,5,AlignmentPatterns);
    vector<int> val{1,0};//默认是白色，1为黑色
    int ReserveArea=1;

    // Step 1: Add the Finder Patterns
    res.setValByArray(finder, val, 0, 0);
    res.setValByArray(finder, val, 0, getSideLen()-7);
    res.setValByArray(finder, val, getSideLen()-7, 0);

    // Step 2: Add the Separators
    for(int i=0;i<8;i++){
        //左上
        res.setValue(7, i, 0);
        res.setValue(i, 7, 0);
        //左下
        res.setValue(getSideLen()-1-i, 7, 0);
        res.setValue(getSideLen()-8, i, 0);
        //右上
        res.setValue(7, getSideLen()-1-i, 0);
        res.setValue(i, getSideLen()-8, 0);
    }

    // Step 3: Add the Alignment Patterns
    for(int i=0;i<7&&version!=1;i++){
        for(int j=0;j<7;j++){
            if(AlignmentPatternLocal[version][i]==0||
                AlignmentPatternLocal[version][j]==0) continue;
            int row=AlignmentPatternLocal[version][i],
                col=AlignmentPatternLocal[version][j];
            //判断是否与finder重合
            if((row-2<8&&col-2<8)||
                (row-2<8&&col+2>getSideLen()-9)||
                (row+2>getSideLen()-9&&col-2<8)) continue;
            res.setValByArray(alignment, val, row-2, col-2);
            // cout<<res<<endl;
        }
    }
    
    // Step 4: Add the Timing Patterns
    for(int i=8;i<getSideLen()-8;i+=2){
        if(res.getValue(i, 6)==-1) res.setValue(i, 6, 1);
        if(res.getValue(6, i)==-1) res.setValue(6, i, 1);
        
        if(res.getValue(i+1, 6)==-1) res.setValue(i+1, 6, 0);
        if(res.getValue(6, i+1)==-1) res.setValue(6, i+1, 0);
    }

    // Step 5: Add the Dark Module and Reserved Areas
    // Dark Module
    res.setValue(4*version+9, 8, 1);
    // Reserve the Format Information Area  set value 2
    for(int i=0;i<9;i++){
        //左上
        if(res.getValue(8, i)==-1) res.setValue(8, i, 2);
        if(res.getValue(i, 8)==-1) res.setValue(i, 8, 2);
        //左下
        if(res.getValue(getSideLen()-1-i, 8)==-1&&i<7) res.setValue(getSideLen()-1-i, 8, 2);
        //右上
        if(res.getValue(8, getSideLen()-1-i)==-1&&i<8) res.setValue(8, getSideLen()-1-i, 2);
    }
    // QR codes versions 7 and larger must contain two areas where version information bits are placed. Each of area 6×3. set value 3
    for(int i=0;i<3&&version>=7;i++){
        for(int j=0;j<6;j++){
            res.setValue(getSideLen()-9-i, j, 3);
            res.setValue(j, getSideLen()-9-i, 3);
        }
    }

    // Step 6: Place the Data Bits
    int r=getSideLen()-1,c=getSideLen()-1,codePos=0;
    bool upWard=true,right=true;
    while(r>=0&&r<getSideLen()&&c>=0&&c<getSideLen()&&codePos<code.size()){
        // cout<<"("<<r<<","<<c<<") ";
        if(res.getValue(r, c)==-1){
            res.setValue(r, c, code[codePos++]-'0'+5);//此处赋值将数据值赋值为5和6，目的是使后面mask方便
            // else
            //     res.setValue(r, c, 5);
        }
        if(upWard){
            if(right) {right=!right; c-=1;}
            else {
                right=!right;
                if (r-1<0) {c-=1;r=0;right=true; upWard=!upWard;}
                else{ c+=1; r-=1;}
            }
        }
        else{
            if(c==6) {c=5; right=true;}
            if(right) {right=!right; c-=1;}
            else {
                right=!right;
                if(r+1>=getSideLen()){c-=1;r=getSideLen()-1;right=true; upWard=!upWard;}
                else {c+=1; r+=1; }
            }
        }
    }
    int a=code.size();

    // mask evaluate
    auto dataMatrix=getMaskList(res);
    int minScore=INT_MAX, minScorePos=0;
    for(int i=0;i<dataMatrix.size();i++){
        //设置保留位
        FormatAndVersionInfo(dataMatrix[i],i);
#ifdef DEBUG
        Matrix<RGB> rgb(getSideLen()+8,getSideLen()+8,BACKGROUND_COLOR);
        rgb.setValByArray(dataMatrix[i], vector<RGB>{RGB_BLACK}, 4, 4);
        BMPData bmp(AmplifyMatrix<RGB>(rgb,AMPLIFY_LEVEL),rgb.col*AMPLIFY_LEVEL,rgb.row*AMPLIFY_LEVEL,true);
        bmp.GrayEncoder();
        string path = "qr"+string(1,'1'+i)+".bmp";
        bmp.saveBMP(path.c_str());
#endif
        // cout<<dataMatrix[i]<<endl;
        int t=Evaluate(dataMatrix[i]);
        if(t<minScore){
            minScore=t;
            minScorePos=i;
        }
        // cout<<t<<" ";
    }
    // cout<<endl;
    res=dataMatrix[minScorePos];

    return res;
}

void QREncode::FormatAndVersionInfo(Matrix<int>& matrix, int maskIndex){
    // Format String
    // int *CurrentCode=getErrorCurrentVersion(level, maskIndex);
    string CurrentCode=FormatInfoList[((int)level)*8+maskIndex];
    for(int i=0;i<7;i++){
        int t=0;
        if(i>=6) t=1;
        matrix.setValue(8, i+t, CurrentCode[i]-'0');
        // left down
        matrix.setValue(getSideLen()-1-i, 8, CurrentCode[i]-'0');
    }
    for(int i=0;i<8;i++){
        // right up
        matrix.setValue(8, getSideLen()-1-i, CurrentCode[14-i]-'0');
        int t=0;
        if(i>=6) t=1;
        matrix.setValue(i+t, 8, CurrentCode[14-i]-'0');
    }
    
    // Version Information
    if(version>=7){
        string CurrentInfo=VersionInfoList[version-7];
        int pos=0;
        for(int i=0;i<3;i++){
            for(int j=0;j<6;j++){
                matrix.setValue(getSideLen()-11+i, j, CurrentInfo[pos]-'0');
                matrix.setValue(j, getSideLen()-11+i, CurrentInfo[pos++]-'0');
            }
        }
    }
}

NAME_SPACE_END()
