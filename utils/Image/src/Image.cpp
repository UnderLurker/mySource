#include "Image.h"
#include "Util.h"
#include <algorithm>
#include <exception>
#include <fstream>
#include <stdint.h>
#include <bitset>
#include <utility>
#include <cstring>
#include <vector>

NAME_SPACE_START(myUtil)

#define ROW 8
#define COL 8

uint16_t ReadByte(fstream& file,int len){
    uint16_t res=file.get();
    if(len!=1){
        res=(res<<8)+(uint8_t)file.get();
    }
    return res;
}

uint16_t findHuffmanCodeByBit(fstream& file,int& length,int& pos,string& deque,int curValue,int& curValLen,bool flag){
    if(length==0 || pos==length){
        if(length==0){
            deque="";
            pos=0;
        }
        #ifdef _DEBUGOUT_
        string str=bitset<8>(file.get()).to_string();
        deque.append(str);
        cout<<str;
        #endif
        #ifndef _DEBUGOUT_
        deque.append(bitset<8>(file.get()).to_string());
        #endif
        length+=8;
    }
    else if(length>=HUFFMAN_DECODE_DEQUE_CACHE){//达到最大缓存
        deque = deque.substr(pos);
        #ifdef _DEBUGOUT_
        string str=bitset<8>(file.get()).to_string();
        deque.append(str);
        cout<<str;
        #endif
        #ifndef _DEBUGOUT_
        deque.append(bitset<8>(file.get()).to_string());
        #endif
        length = deque.length();
        pos = 0;
    }
    uint8_t temp=(uint8_t)(deque.at(pos++) - '0');
    if(flag){//true 取反
        temp = (temp&1)^1;
    }
    curValue = (curValue << 1) + temp;
    curValLen++;
    return curValue;
}

double** UnZigZag(int* originArray){
    double** table=new double*[ROW];
    for(int i=0;i<ROW;i++) table[i]=new double[COL];
    int cur=0,x=0,y=0;
    bool flag = true;//true是右上 false是左下
    while (cur < 64) {
        table[y][x] = originArray[cur++];
        if (flag) { x++; y--; }
        else { x--; y++; }
        if (x < 0 || y < 0 || x>7 || y>7) flag = !flag;
        if (x < 0 && y>7) { x = 1; y = 7; }
        if (x < 0) x = 0;
        else if (x > 7) { x = 7; y += 2; }
        if (y < 0) y = 0;
        else if (y > 7) { y = 7; x += 2; }
    }
    #ifdef _DEBUGOUT_
    cout << endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout.width(3);
            cout << dec << table[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    #endif
    return table;
}

void IDCT(double** originMatrix){
    vector<vector<double>> temp(ROW,vector<double>(COL,0));
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            double sum=0;
            for(int k=0;k<COL;k++){
                sum+=IDctArray[k][i]*originMatrix[k][j];
            }
            temp[i][j]=sum;
        }
    }
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            double sum=0;
            for(int k=0;k<COL;k++){
                sum+=temp[i][k]*IDctArray[k][j];
            }
            originMatrix[i][j]=sum;
        }
    }
    #ifdef _DEBUGOUT_
    cout << endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout.width(3);
            cout << dec << originMatrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    #endif
}


bool JPEGScan::Init(fstream &file, uint16_t len){
    try {
        uint8_t count=file.get();
        len--;
        while(count--){
            uint8_t componentId=file.get();
            uint8_t table=file.get();
            uint8_t dcId=table>>4;
            uint8_t acId=table&0x0f;
            pair<uint8_t, uint8_t> info1(dcId,acId);
            pair<uint8_t, pair<uint8_t, uint8_t>> info2(componentId,info1);
            componentHuffmanMap.insert(info2);
        }
    } catch (...) {
        return false;
    }
    return true;
}

bool JPEGHuffmanCode::Init(fstream &file, uint16_t len){
    try{
        vector<uint8_t> temp;
        while(len--){
            int info=file.get();
            temp.push_back(info);
            cout<<dec<<info<<" ";
        }
        int curPos = 16, curCode = 0;
        for (int i = 0; i < 16; i++) {
            int count = temp[i];
            curCode <<= 1;
            while (count--) {
                uint16_t code=curCode;
                uint8_t bit=i+1;
                uint8_t weight=temp[curPos];
                pair<uint8_t, uint8_t> t1(bit,weight);
                pair<uint16_t, pair<uint8_t, uint8_t>> t2(curCode,t1);
                table.insert(t2);
                curCode++;
                curPos++;
            }
        }

    }
    catch(...){
        return false;
    }
    return true;
}

bool JPEGHuffmanCode::findKey(const uint16_t& code,const uint8_t& bit,iterator& it)
{
    it=table.find(code);
    if(it==table.end()) return true;
    return it->second.first!=bit;
}
bool JPEGQuality::Init(fstream &file, uint16_t len){
    try{
        int info=file.get();
        precision=info>>4;
        id=info&0x0f;
        len--;
        while(len--){
            int t=file.get();
            table.push_back(t);
        }
    }
    catch(...){
        return false;
    }
    return true;
}

bool JPEGComponent::Init(fstream &file, uint16_t len){
    try {
        int info1=file.get();
        int info2=file.get();
        int info3=file.get();
        colorId=info1;
        h_samp_factor=info2>>4;
        v_samp_factor=info2&0x0f;
        qualityId=info3;
    } catch (...) {
        return false;
    }
    return true;
}

bool JPEGData::readJPEG(const char *filePath){
    fstream file(filePath,ios::in|ios::binary);
    if(file.fail()) return false;
    file.seekg(2,ios::beg);
    dc_huffman.resize(2);
    ac_huffman.resize(2);
    try {
        //do read data through using other method
        uint16_t pLen=0;
        uint16_t pMarker=0xFF;
        uint16_t pType=0x00;
        while(!file.eof()){
            pMarker=file.get();
            pType=file.get();
            if(pType==EOI) break;

            pLen=file.get();
            pLen=(pLen<<8)+file.get();
            cout<<hex<<pMarker<<" "<<pType<<" "<<pLen<<endl;
            if(pMarker!=0xFF) throw exception();
            bool flag=true;
            switch (pType) {
                case SOF0:
                case SOF1:
                case SOF2:{
                    flag=readSOF(file, pLen-2);
                    break;
                }
                case DHT:{
                    JPEGHuffmanCode huf;
                    int info=file.get();
                    int tableId=info&0x0f;
                    cout<<hex<<info<<" ";
                    flag=huf.Init(file, pLen-3);
                    if((info>>4)&1) ac_huffman[tableId]=huf;
                    else dc_huffman[tableId]=huf;
                    break;
                }
                //case SOI:
                //case EOI:
                case SOS:{
                    flag=scan.Init(file, pLen-2);
                    int count=3;
                    cout<<endl;
                    while(count--) 
                        cout<<hex<<file.get();
                    cout<<endl;
                    //正式读取数据
                    if(!flag) break;
                    flag=readData(file);
                    break;
                }
                case DQT:{
                    JPEGQuality q;
                    flag=q.Init(file, pLen-2);
                    quality.push_back(q);
                    break;
                }
                case DRI:{
                    resetInterval=ReadByte(file, 2);
                    break;
                }
                case APP0:
                case APP1:
                case APP2:
                case COM:{
                    pLen-=2;
                    while(pLen--){
                        file.get();
                    }
                    break;
                }
                default:
                    pLen-=2;
                    while(pLen--){
                        file.get();
                    }
                    break;
                
            }
            if(!flag) throw exception();
            cout<<endl;
        }
    } catch (...) {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool JPEGData::readSOF(fstream& file,uint16_t len){
    try {
        precision=file.get();
        height=max(height,(int)ReadByte(file, 2));
        width=max(width,(int)ReadByte(file, 2));
        int count=ReadByte(file, 1);
        if(count!=3) return false;
        len-=6;
        component.resize(count);
        for(int i=0;i<count;i++){
            JPEGComponent com;
            com.Init(file, len/3);
            max_h_samp_factor=max(max_h_samp_factor,(int)com.h_samp_factor);
            max_v_samp_factor=max(max_v_samp_factor,(int)com.v_samp_factor);
            component[i]=com;
        }
        if((component[0].h_samp_factor*component[0].v_samp_factor)
            /(component[1].h_samp_factor*component[1].v_samp_factor)==4){
            isYUV411=true;
        }
        else if((component[0].h_samp_factor*component[0].v_samp_factor)
            /(component[1].h_samp_factor*component[1].v_samp_factor)==2){
            isYUV422=true;
        }
        else if((component[0].h_samp_factor*component[0].v_samp_factor)
            /(component[1].h_samp_factor*component[1].v_samp_factor)==1){
            isYUV111=true;
        }
    } catch (...) {
        return false;
    }
    return true;
}

bool JPEGData::readData(fstream& file){
    bool flag=true;
    try{
        //使用huffman表来解出RLE编码，接着转回长度为64的矩阵
        flag=huffmanDecode(file);
        if(!flag) return false;
        //反量化，即上面的64矩阵×对应位置的量化表
        //flag=deQuantity();
        //if(!flag) return false;
        //反zig-zag排序
        //flag=deZSort();
        //if(!flag) return false;
        //反离散余弦变换

        //if(!flag) return false;
        //YCbCr转RGB

        //if(!flag) return false;
    }
    catch(...){
        return false;
    }
    return true;
}

bool JPEGData::huffmanDecode(fstream& file){
    try {
        //原图像一个MCU有多少8*8矩阵（此时是YCbCr还没有分开）
        //int MCUBlockCount=max_h_samp_factor*max_v_samp_factor;
        //顺序YCbCr
        int YUV[]={component[0].h_samp_factor*component[0].v_samp_factor,
                    component[1].h_samp_factor*component[1].v_samp_factor,
                    component[2].h_samp_factor*component[2].v_samp_factor};
        int curMCUCount=0;      //当前MCU数量，为了重置直流分量
        int preDCValue=0;       //上一个DC分量
        int curValueLength=0;           //当前值有多少位
        int curValue=0;         //当前的值

        int curBitDequeLength=8;//当前curBitDeque长度
        int curBitPos=0;        //当前string读取到第几位
        string curBitDeque="";  //用来存储读出来的2进制数
        //一次循环解析一个MCU
        curBitDeque.append(bitset<8>(file.get()).to_string());
        curBitDequeLength=8;
        cout<<curBitDeque;
        while(1){
            cout<<endl;
            int** MCUStruct=new int*[6];
            int count=0;
            for(int i=0;i<3;i++){
                for(int j=0;j<YUV[i];j++){
                    int matrix[64]={0};
                    //memset(matrix, 0, sizeof(int)*64);
                    int valCount=0;
                #ifdef _DEBUGOUT_
                    cout<<endl;
                #endif
                    uint8_t dcID = scan.componentHuffmanMap[component[i].colorId].first;
                    uint8_t acID = scan.componentHuffmanMap[component[i].colorId].second;
                    int qualityId=component[i].qualityId;
                    while(valCount<64){
                        //用curBitDeque和curBit去找权重，curValue作为当前键值
                        JPEGHuffmanCode::iterator it;
                        JPEGHuffmanCode &huffman = valCount==0?dc_huffman[dcID]:ac_huffman[acID];
                        while(huffman.findKey(curValue,curValueLength,it)){
                            curValue=findHuffmanCodeByBit(file,curBitDequeLength,curBitPos,curBitDeque,curValue,curValueLength,false);
                        }
                    #ifdef _DEBUGOUT_
                        cout<<"code: "<<dec<<curBitDequeLength-curBitPos<<" ";
                        cout<<dec<<curValue<<" "<<curValueLength<<"\n";
                    #endif
                        //已经找到了权重和位宽
                        uint8_t weight,zeroCount;
                        if(valCount==0) weight = it->second.second;
                        else { weight = it->second.second & 0x0f; zeroCount = it->second.second >> 4;}
                        curValue=0;//这里变为dc或ac值
                        curValueLength=0;
                        bool flag=false;//是否取反
                        if(valCount!=0&&weight==0) break;//后面全是0
                        while(weight--){
                            curValue=findHuffmanCodeByBit(file,curBitDequeLength,curBitPos,curBitDeque,curValue,curValueLength,flag);
                            if(curValueLength==1&&curValue==0){
                                curValue=(curValue&1)^1;
                                flag=true;
                            }
                        }
                    #ifdef _DEBUGOUT_
                        cout<<"weight: "<<dec<<curBitDequeLength-curBitPos<<" ";
                        cout<<dec<<curValue<<" "<<curValueLength<<endl;
                    #endif
                        if(flag) curValue=-curValue;
                        int writeValue=valCount==0?(preDCValue+=curValue):curValue;
                        writeValue*=quality[qualityId].table[valCount];//反量化
                        if(valCount==0) matrix[valCount]=writeValue;
                        else{
                            while(zeroCount--) matrix[valCount++]=0;
                            matrix[valCount]=writeValue;
                        }
                        curValue=0;
                        curValueLength=0;
                        valCount++;
                        if(YUV[0]!=4){
                            cout<<YUV[0]<<endl;
                        }
                    }
                    //MCUStruct[(count++)%6]=matrix;
                    double** tempZ = UnZigZag(matrix);//反zig-zag编码
                    IDCT(tempZ);                    //dct逆变换
                    ycbcr.push_back(tempZ);
                    //delete [] matrix;
                #ifdef _DEBUG_
                    for(int k=0;k<ROW;k++){
                        for(int l=0;l<COL;l++){
                            cout.width(3);
                            cout<<dec<<tempZ[k][j]<<" ";
                        }
                        cout<<endl;
                    }
                    cout<<endl;
                #endif
                    cout<<count++<<" ";
                }
            }
            if(count!=6){
                cout<<" ";
            }
            int** lpRGB = YCbCrToRGB(YUV,curMCUCount);
            rgb.push_back(lpRGB);
            curMCUCount++;
            cout<<"curMCUCount="<<dec<<curMCUCount;
            if(curMCUCount==resetInterval) preDCValue=0;
        }
    } catch (exception ex) {
        cout<<ex.what();
        return false;
    }
    return true;
}

bool JPEGData::deQuantity(){
    try {
        
    } catch (...) {
        return false;
    }
    return true;
}

bool JPEGData::deZSort(){
    try {
        
    } catch (...) {
        return false;
    }
    return true;
}

int** JPEGData::YCbCrToRGB(const int* YUV,int curMCUCount){
    int **res = new int *[ROW * max_v_samp_factor];
    int matrixCount = YUV[0] + YUV[1] + YUV[2];
    int crCount = 0, cbCount = 0;
    //1＝Y, 2＝Cb, 3＝Cr
    //式子 scale*x,scale*y
    int cb_h_samp_scale=component[1].h_samp_factor/max_h_samp_factor,
        cb_v_samp_scale=component[1].v_samp_factor/max_v_samp_factor,
        cr_h_samp_scale=component[2].h_samp_factor/max_h_samp_factor,
        cr_v_samp_scale=component[2].v_samp_factor/max_v_samp_factor;
    for (int i = 0; i < ROW; i++)
        res[i] = new int[COL * max_h_samp_factor];
    // for (int i = 0; i < YUV[0]; i++)
    // {
    //     double **y = ycbcr[(curMCUCount * matrixCount) + i];
    //     double **cr = ycbcr[(curMCUCount * matrixCount) + YUV[0] + crCount];
    //     double **cb = ycbcr[(curMCUCount * matrixCount) + YUV[0] + YUV[1] + cbCount];
    //     if (i != 0 && i % crCountFactor == 0)
    //         crCount++;
    //     if (i != 0 && i % cbCountFactor == 0)
    //         cbCount++;
    //     for(int j=0;j<ROW * max_v_samp_factor;j++){
    //         for(int k=0;k<COL * max_h_samp_factor;k++){
    //         }
    //     }
    // }
    //此处直接生成rgb值
    //注意，此处YCrCb的对应关系与采样因子有关
    for(int j=0;j<ROW * max_v_samp_factor;j++){
        for(int k=0;k<COL * max_h_samp_factor;k++){
            int cur_h_pos=k*cb_h_samp_scale,
                cur_v_pos=j*cb_v_samp_scale;
            double** y=ycbcr[(j/ROW)*component[0].h_samp_factor+(k/COL)*component[0].v_samp_factor];
            double** cb=ycbcr[YUV[0]];
            double** cr=ycbcr[YUV[0]+YUV[1]];
        }
    }
    ycbcr.clear();
    return res;
}

NAME_SPACE_END()
