#include "Image.h"
#include "Util.h"
#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <stdint.h>
#include <bitset>
#include <stdlib.h>
#include <utility>
#include <cstring>
#include <vector>
#include <iomanip>

NAME_SPACE_START(myUtil)

//获取Two-dimensional Gaussian distribution
double** getTwoDimGaussianDistrbute(int GaussianRadius,int GaussianTemplateRadius){
    int n=2*GaussianTemplateRadius+1;
    double** res=new double*[n];
    for(int i=0;i<n;i++) res[i]=new double[n];
    double t=1.0/(2*M_PI*GaussianRadius*GaussianRadius);
    for(int i=-1*GaussianTemplateRadius;i<=GaussianTemplateRadius;i++){
        for(int j=-1*GaussianTemplateRadius;j<=GaussianTemplateRadius;j++){
            res[i+GaussianTemplateRadius][j+GaussianTemplateRadius]=t*exp((-1)*(i*i+j*j)/(2*GaussianRadius*GaussianRadius));
        }
    }
    return res;
}

int RGBValueLimit(double input){
    if(input<0) return 0;
    else if(input>255) return 255;
    // 四舍五入、取整均可
    // return (int)(input);
    return round(input);
}

void print(double** originMatrix){
    cout<<endl;
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            cout<<originMatrix[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}

int getBitLength(int num){
    int res=0;
    while(num){
        res+=1;
        num>>=1;
    }
    return res;
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
    return table;
}

double* ZigZag(double** originArray){
    double* res=new double[ROW*COL];
    int cur=0,x=0,y=0;
    bool flag = true;//true是右上 false是左下
    while (cur < 64) {
        res[cur++]=originArray[y][x];
        if (flag) { x++; y--; }
        else { x--; y++; }
        if (x < 0 || y < 0 || x>7 || y>7) flag = !flag;
        if (x < 0 && y>7) { x = 1; y = 7; }
        if (x < 0) x = 0;
        else if (x > 7) { x = 7; y += 2; }
        if (y < 0) y = 0;
        else if (y > 7) { y = 7; x += 2; }
    }
    return res;
}

void writeByte(fstream& file,uint8_t val){
    file.write((char*)&val, 1);
}

void writeTwoByte(fstream& file,uint16_t val){
    writeByte(file, val>>8);
    writeByte(file, val&0xFF);
}

#define WRITE_BIT_TO_FILE(file, bitCurPos, curValue, len, realData)            \
  while (len > (8 - bitCurPos)) {                                              \
    int rightMoveBit = len + bitCurPos - 8;                                    \
    int bitValue = realData >> rightMoveBit;                                   \
    curValue |= bitValue;                                                      \
    writeByte(file, (uint8_t)curValue);                                        \
    realData -= bitValue << rightMoveBit;                                      \
    len -= 8 - bitCurPos;                                                      \
    curValue = bitCurPos = 0;                                                  \
  }                                                                            \
  curValue |= realData << (8 - bitCurPos - len);                               \
  bitCurPos += len;                                                            \
  if (bitCurPos >= 8) {                                                        \
    writeByte(file, (uint8_t)curValue);                                        \
    curValue = bitCurPos = 0;                                                  \
  }

static int bitCurPos = 0;   // 当前字节在哪个bit位
static int curBitValue = 0;    // 当前值是多少
void writeBit(fstream& file,int realData,pair<uint16_t,uint8_t>& huffmanCode){

    int codeLen=huffmanCode.second,code=huffmanCode.first;
    WRITE_BIT_TO_FILE(file, bitCurPos, curBitValue, codeLen, code)

    if (realData == 0)
        return;
    int len = getBitLength((int)abs(realData));
    WRITE_BIT_TO_FILE(file, bitCurPos, curBitValue, len, realData)
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

void JPEGHuffmanCode::write(fstream &file){
    // Y dc
    writeByte(file, (uint8_t)FLAG);
    writeByte(file, (uint8_t)DHT);
    writeTwoByte(file, (uint16_t)0x001F);
    writeByte(file, (uint8_t)0x00);
    for(int i=0;i<17;i++) writeByte(file, bits_dc_luminance[i]);
    for(int i=0;i<12;i++) writeByte(file, val_dc_luminance[i]);
    // Y ac
    writeByte(file, (uint8_t)FLAG);
    writeByte(file, (uint8_t)DHT);
    writeTwoByte(file, (uint16_t)0x00B5);
    writeByte(file, (uint8_t)0x10);
    for(int i=0;i<17;i++) writeByte(file, bits_ac_luminance[i]);
    for(int i=0;i<162;i++) writeByte(file, val_ac_luminance[i]);
    // UV dc
    writeByte(file, (uint8_t)FLAG);
    writeByte(file, (uint8_t)DHT);
    writeTwoByte(file, (uint16_t)0x001F);
    writeByte(file, (uint8_t)0x01);
    for(int i=0;i<17;i++) writeByte(file, bits_dc_chrominance[i]);
    for(int i=0;i<12;i++) writeByte(file, val_dc_chrominance[i]);
    // UV ac
    writeByte(file, (uint8_t)FLAG);
    writeByte(file, (uint8_t)DHT);
    writeTwoByte(file, (uint16_t)0x00B5);
    writeByte(file, (uint8_t)0x11);
    for(int i=0;i<17;i++) writeByte(file, bits_ac_chrominance[i]);
    for(int i=0;i<162;i++) writeByte(file, val_ac_chrominance[i]);
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

void JPEGQuality::write(fstream &file){
    writeByte(file, FLAG);
    writeByte(file, DQT);
    writeTwoByte(file, (uint16_t)0x0043);
    writeByte(file, (uint8_t)0x00);
    for(int i=0;i<64;i++){
        writeByte(file, YQualityTable[i]);
    }
    writeByte(file, FLAG);
    writeByte(file, DQT);
    writeTwoByte(file, (uint16_t)0x0043);
    writeByte(file, (uint8_t)0x01);
    for(int i=0;i<64;i++){
        writeByte(file, CQualityTable[i]);
    }
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
    file.seekg(0,ios::end);
    pos = file.tellg();
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
            // cout<<hex<<pMarker<<" "<<pType<<" "<<pLen<<endl;
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
                    // cout<<hex<<info<<" ";
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
                    // cout<<endl;
                    while(count--) file.get();
                    // cout<<endl;
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
            // cout<<endl;
        }
    } catch (...) {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool JPEGData::writeJPEG(const char* filePath, int samp_factor[3][2]){
    max_h_samp_factor=0;
    max_v_samp_factor=0;
    for(int i=0;i<3;i++){
        write_samp_factor[i][0]=samp_factor[i][0];
        write_samp_factor[i][1]=samp_factor[i][1];
        max_h_samp_factor=max(max_h_samp_factor,write_samp_factor[i][0]);
        max_v_samp_factor=max(max_v_samp_factor,write_samp_factor[i][1]);
    }
    fstream file(filePath,ios::out|ios::binary);
    if(file.fail()) return false;
    try{
        createDCEnHuffman();
        createACEnHuffman();
        writeTwoByte(file, (uint16_t)((FLAG << 8) + SOI));  // SOI
        JPEGQuality::write(file);                           // DQT
        writeTwoByte(file, (uint16_t)((FLAG << 8) + SOF0)); // SOF
        writeTwoByte(file, (uint16_t)0x0011);
        writeByte(file, (uint8_t)0x08);
        writeTwoByte(file, (uint16_t)height);
        writeTwoByte(file, (uint16_t)width);
        writeByte(file, (uint8_t)0x03);

        writeByte(file, (uint8_t)0x01);
        writeByte(file, (uint8_t)((samp_factor[0][0] << 4) + samp_factor[0][1]));
        writeByte(file, (uint8_t)0x00);

        writeByte(file, (uint8_t)0x02);
        writeByte(file, (uint8_t)((samp_factor[1][0] << 4) + samp_factor[1][1]));
        writeByte(file, (uint8_t)0x01);

        writeByte(file, (uint8_t)0x03);
        writeByte(file, (uint8_t)((samp_factor[2][0] << 4) + samp_factor[2][1]));
        writeByte(file, (uint8_t)0x01);

        JPEGHuffmanCode::write(file);   // DHT
        writeTwoByte(file, (uint16_t)((FLAG << 8) + DRI)); // DRI
        writeTwoByte(file, (uint16_t)0x0004);
        writeTwoByte(file, (uint16_t)mcu_cols);
        writeTwoByte(file, (uint16_t)((FLAG<<8)+SOS)); // SOS
        writeTwoByte(file, (uint16_t)0x000C);
        writeByte(file, (uint8_t)0x03);

        writeByte(file, (uint8_t)0x01);
        writeByte(file, (uint8_t)0x00);
        writeByte(file, (uint8_t)0x00);
        
        writeByte(file, (uint8_t)0x02);
        writeByte(file, (uint8_t)0x01);
        writeByte(file, (uint8_t)0x01);

        writeByte(file, (uint8_t)0x03);
        writeByte(file, (uint8_t)0x01);
        writeByte(file, (uint8_t)0x01);

        writeByte(file, (uint8_t)0);
        writeByte(file, (uint8_t)0);
        writeByte(file, (uint8_t)0);
        RGBToYCbCr(getRGBMatrix(), file);
        writeTwoByte(file, (uint16_t)((FLAG << 8) + (uint8_t)JPEGPType::EOI));  // EOI
    }
    catch(...){
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
        mcu_rows=ceil(height/(8*max_v_samp_factor));
        mcu_cols=ceil(width/(8*max_h_samp_factor));
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
        int curMCUCount=1;      //当前MCU数量
        int curValueLength=0;   //当前值有多少位
        int curValue=0;         //当前的值

        int curBitDequeLength=8;//当前curBitDeque长度
        int curBitPos=0;        //当前string读取到第几位
        int restart=resetInterval;//直流分量重置
        string curBitDeque="";  //用来存储读出来的2进制数
        //一次循环解析一个MCU
        curBitDeque.append(bitset<8>(file.get()).to_string());
        curBitDequeLength=8;
        // cout<<curBitDeque;
        while(!EOI||(pos-file.tellg())!=2){
            // cout<<endl;
            int count=1;
            for(int i=0;i<3;i++){
                for(int j=0;j<YUV[i];j++){
                    // cout<<count++<<" ";
                    int matrix[64]={0};
                    int valCount=0;
                    uint8_t dcID = scan.componentHuffmanMap[component[i].colorId].first;
                    uint8_t acID = scan.componentHuffmanMap[component[i].colorId].second;
                    int qualityId=component[i].qualityId;
                    if(qualityId>=quality.size()) qualityId=0;
                    // cout<<endl;
                    while(valCount<64){
                        //用curBitDeque和curBit去找权重，curValue作为当前键值
                        JPEGHuffmanCode::iterator it;
                        JPEGHuffmanCode &huffman = valCount==0?dc_huffman[dcID]:ac_huffman[acID];
                        while(curValueLength<=16&&huffman.findKey(curValue,curValueLength,it)){
                            curValue=findHuffmanCodeByBit(file,curBitDequeLength,curBitPos,curBitDeque,curValue,curValueLength);
                        }
                        if(curValueLength>16) 
                            return true;
                        #ifdef _DEBUGOUT_
                        //cout<<dec<<" "<<curBitPos<<" "<<curBitDequeLength<<" ";
                        cout<<"key="<<hex<<curValue<<" len="<<curValueLength<<endl;
                        #endif
                        //已经找到了权重和位宽
                        uint8_t weight,zeroCount=0;
                        if(valCount==0) weight = it->second.second;
                        else { weight = it->second.second & 0x0f; zeroCount = it->second.second >> 4;}
                        curValue=0;//这里变为dc或ac值
                        curValueLength=0;
                        if(valCount!=0&&weight==0&&zeroCount==0) break;//后面全是0
                        // 读取真正的值
                        for(int k=0;k<weight;k++){
                            curValue=findHuffmanCodeByBit(file,curBitDequeLength,curBitPos,curBitDeque,curValue,curValueLength);
                        }
                        curValue = (curValue >= pow(2, curValueLength - 1) ? curValue : curValue - pow(2, curValueLength) + 1);
                        // cout<<curValue<<endl;
                        int writeValue=valCount==0?(preDCValue[i]+=curValue):curValue;
                        valCount+=zeroCount;
                        writeValue*=quality[qualityId].table[valCount];//反量化
                        matrix[valCount]=writeValue;
                        curValue=0;
                        curValueLength=0;
                        valCount++;
                    }
                    double** tempZ = UnZigZag(matrix);//反zig-zag编码
                    //反量化，在反zig-zag编码前后差别，前面：RGB数值与编辑器比偏小，反之偏大，这也与最后取整时的方式有关
                    // deQuality(tempZ,qualityId);
                    // print(tempZ);
                    //隔行正负纠正，有的博客说了，但是没感觉有啥帮助
                    // PAndNCorrect(tempZ);
                    IDCT(tempZ);                    //dct逆变换
                    ycbcr.push_back(tempZ);
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
                }
            }
            // if(count!=6){
            //     cout<<" ";
            // }
            RGB** lpRGB = YCbCrToRGB(YUV);
            FREE_VECTOR_LP(ycbcr,ROW)
            rgb.push_back(lpRGB);
            // 直流分量重置间隔不为0的
            if(restart>0){
                resetInterval--;
                if(resetInterval==0){
                    resetInterval=restart;
                    curDRI+=1;
                    curDRI&=0x7;
                    //需要在此处读取两字节信息，看是否为重置标识
                    file.get();
                    if(file.get()==0xD9) EOI=true;
                    curBitPos=curBitDequeLength;
                    preDCValue[0]=0;
                    preDCValue[1]=0;
                    preDCValue[2]=0;
                }
            }
            // cout<<"curMCUCount="<<dec<<curMCUCount++<<" pos="<<pos<<"/"<<file.tellg()<<" "<<file.tellg()*100.0/pos<<"%\n";
            if(pos-file.tellg()==2) break;
        }
        cout<<"\nsuccessfully\n";
    } catch (exception ex) {
        cout<<ex.what();
        return false;
    }
    return true;
}

RGB** JPEGData::YCbCrToRGB(const int* YUV){
    RGB **res = new RGB *[ROW * max_v_samp_factor];
    int matrixCount = YUV[0] + YUV[1] + YUV[2];
    int crCount = 0, cbCount = 0;
    //1＝Y, 2＝Cb, 3＝Cr
    //式子 scale*x,scale*y
    double cb_h_samp_scale=component[1].h_samp_factor*1.0/max_h_samp_factor,
           cb_v_samp_scale=component[1].v_samp_factor*1.0/max_v_samp_factor,
           cr_h_samp_scale=component[2].h_samp_factor*1.0/max_h_samp_factor,
           cr_v_samp_scale=component[2].v_samp_factor*1.0/max_v_samp_factor;
    for (int i = 0; i < ROW * max_v_samp_factor; i++)
        res[i] = new RGB[COL * max_h_samp_factor];
    //此处直接生成rgb值
    //注意，此处YCbCr的对应关系与采样因子有关
    // cout<<endl;
    for(int j=0;j<ROW * max_v_samp_factor;j++){
        for(int k=0;k<COL * max_h_samp_factor;k++){
            int yPos = (j / ROW) * component[0].h_samp_factor + (k / COL);
            int cbPos = YUV[0] + (int)((k / COL) * cb_v_samp_scale) + (int)((j / ROW) * cb_h_samp_scale);
            int crPos = YUV[0] + YUV[1] + (int)((k / COL) * cr_v_samp_scale) + (int)((j / ROW) * cr_h_samp_scale);
            double y = ycbcr[yPos][j % ROW][k % COL];
            double cb = ycbcr[cbPos][(int)(j * cb_v_samp_scale)][(int)(k * cb_h_samp_scale)];
            double cr = ycbcr[crPos][(int)(j * cr_v_samp_scale)][(int)(k * cr_h_samp_scale)];

            res[j][k].red   =RGBValueLimit(128+y+1.402  *cr);
            res[j][k].green =RGBValueLimit(128+y-0.71414*cr-0.34414*cb);
            res[j][k].blue  =RGBValueLimit(128+y+1.772  *cb);

            
            // 输出当前选择的矩阵
            //cout<<dec<<yPos<<" "<<cbPos<<" "<<crPos<<" ";
            // cout<<hex<<setw(2)<<setfill('0')<<(int)res[j][k].red
            //          <<setw(2)<<setfill('0')<<(int)res[j][k].green
            //          <<setw(2)<<setfill('0')<<(int)res[j][k].blue<<" ";
        }
        // cout<<endl;
    }
    // cout<<endl;
    return res;
}

void JPEGData::RGBToYCbCr(Matrix<RGB> _rgb, fstream& file){
    // vector<double*> res;
    int mcu_width  = COL * max_h_samp_factor,
        mcu_height = ROW * max_v_samp_factor;
    int YUV[3] = {write_samp_factor[0][0] * write_samp_factor[0][1],
                  write_samp_factor[1][0] * write_samp_factor[1][1],
                  write_samp_factor[2][0] * write_samp_factor[2][1]};
    int y_h_param  = max_h_samp_factor * 1.0 / write_samp_factor[0][0],
        y_v_param  = max_v_samp_factor * 1.0 / write_samp_factor[0][1],
        cb_h_param = max_h_samp_factor * 1.0 / write_samp_factor[1][0],
        cb_v_param = max_v_samp_factor * 1.0 / write_samp_factor[1][1],
        cr_h_param = max_h_samp_factor * 1.0 / write_samp_factor[2][0],
        cr_v_param = max_v_samp_factor * 1.0 / write_samp_factor[2][1];
    double cb_h_samp_scale=write_samp_factor[1][0]*1.0/max_h_samp_factor,
           cb_v_samp_scale=write_samp_factor[1][1]*1.0/max_v_samp_factor,
           cr_h_samp_scale=write_samp_factor[2][0]*1.0/max_h_samp_factor,
           cr_v_samp_scale=write_samp_factor[2][1]*1.0/max_v_samp_factor;
    preDCValue[0] = preDCValue[1] = preDCValue[2] = 0;
    mcu_rows=ceil(_rgb.row*1.0/mcu_height);
    mcu_cols=ceil(_rgb.col*1.0/mcu_width);
    int n = mcu_rows * mcu_cols, nSize = 0, DRI = mcu_cols, DriFLAG = 0xD0;
    // res.resize(n);
    while(n>0){
        double*** yuv=new double**[YUV[0]+YUV[1]+YUV[2]];
        int count=0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < YUV[i]; j++) {
                yuv[count] = new double *[ROW];
                for (int k = 0; k < ROW; k++) {
                    yuv[count][k] = new double[COL];
                    memset(yuv[count][k], 0.0, sizeof(double) * COL);
                }
                count++;
            }
        }
        int row = nSize * mcu_height / mcu_cols,
            col = (nSize % mcu_cols) * mcu_width;
        for (int i = 0; i < mcu_height; i++) {
            for (int j = 0; j < mcu_width; j++) {
                RGB t = _rgb.getValue(row+i, col+j);//得到的是一整个mcu，但是要把它分成多个8*8矩阵
                double y  =  0.299 * t.red + 0.587 * t.green + 0.114 * t.blue;
                double cb = -0.147 * t.red - 0.289 * t.green + 0.436 * t.blue;
                double cr =  0.615 * t.red - 0.515 * t.green - 0.100 * t.blue;
                int yPos = (i / ROW) * max_h_samp_factor + (j / COL);
                int cbPos = YUV[0] + (int)((j / COL) * cb_v_samp_scale) +
                            (int)((i / ROW) * cb_h_samp_scale);
                int crPos = YUV[0] + YUV[1] +
                            (int)((j / COL) * cr_v_samp_scale) +
                            (int)((i / ROW) * cr_h_samp_scale);

                if (i % y_v_param == 0 && j % y_h_param == 0)
                    yuv[yPos][i % ROW][j % COL] = y;
                if (i % cb_v_param == 0 && j % cb_h_param == 0)
                    yuv[cbPos][i % ROW][j % COL] = cb;
                if (i % cr_v_param == 0 && j % cr_h_param == 0)
                    yuv[crPos][i % ROW][j % COL] = cr;
            }
        }
        int pos = 0;
        for (int i = 0; i < 3; i++) {
            int huffmanID = i == 0 ? 0 : 1;
            for (int j = 0; j < YUV[i]; j++) {
                // for(int k=0;k<)
                DCT(yuv[pos]);
                double *temp = ZigZag(yuv[pos++]);
                Quality(temp, i >= YUV[0] ? 2 : 1);
                int zeroCount=0;
                for(int k=0;k<64;k++){
                    temp[k] = round(temp[k]);
                    if (k == 0) // 下面进行差分编码
                        temp[0] -= preDCValue[i];
                    int len = getBitLength((int)abs(temp[k]));
                    if (temp[k] < 0){
                        temp[k] = pow(2, len) + temp[k] - 1;
                        len = getBitLength((int)abs(temp[k]));
                    }
                    if(k==0) writeBit(file, temp[k], en_dc_huffman[huffmanID].table[len]);
                    else if(temp[k]==0){ zeroCount++; continue;}
                    else{ writeBit(file, temp[k], en_ac_huffman[huffmanID].table[(zeroCount << 4) + len]);zeroCount = 0;}
                }
                if(zeroCount!=0) writeBit(file, 0, en_ac_huffman[huffmanID].table[0]);
                if (nSize % DRI == 0) {
                    preDCValue[0] = preDCValue[1] = preDCValue[2] = 0;
                    writeTwoByte(file, (uint16_t)((FLAG << 8) + DriFLAG++));
                    if (DriFLAG > 0xD7)
                        DriFLAG = 0xD0;
                }
                if (curBitValue != 0)
                    writeByte(file, (uint8_t)curBitValue);
                bitCurPos = curBitValue = 0;
            }
        }
        FREE_LP_3(yuv, YUV[0] + YUV[1] + YUV[2], ROW)
        n--;
        nSize++;
    }
    // return res;
}

Matrix<RGB> JPEGData::getRGBMatrix(){
    Matrix<RGB> res(height,width);
    int mcu_height=ROW*max_v_samp_factor,
        mcu_width=COL*max_h_samp_factor;
    for(int row=0;row<height;row++){
        int rowOffset=(int)floor(row*1.0/mcu_height)*(int)ceil(width*1.0/mcu_width);
        for(int col=0;col<width;col++){
            int pos=rowOffset+col/(8*max_h_samp_factor);
            if(pos>=rgb.size()) pos=rgb.size()-1;
            res.setValue(row,col,rgb[pos][row%mcu_height][col%mcu_height]);
        }
    }
    return res;
}

double** JPEGData::createDCTAndIDCTArray(int row){
    double** res=new double*[row];
    for(int i=0;i<row;i++) res[i]=new double[row];
    // cout<<endl;
    for(int i=0;i<row;i++){
        for(int j=0;j<row;j++){
            double t=0;
            if(i==0) t=sqrt(1.0/row);
            else t=sqrt(2.0/row);
            res[i][j]=t*cos(M_PI*(j+0.5)*i/row);
            // cout<<res[i][j]<<" ";
        }
        // cout<<endl;
    }
    return res;
}

void JPEGData::DCT(double** originMatrix){
    //原理 Y=A*X*A'
    vector<vector<double>> temp(ROW,vector<double>(COL,0));
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            double sum=0;
            for(int k=0;k<COL;k++){
                sum+=DCTAndIDCTArray[i][k]*originMatrix[k][j];
            }
            temp[i][j]=sum;
        }
    }
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            double sum=0;
            for(int k=0;k<COL;k++){
                sum+=temp[i][k]*DCTAndIDCTArray[j][k];
            }
            originMatrix[i][j]=sum;
        }
    }
}

void JPEGData::IDCT(double** originMatrix){
    //原理X=A'*Y*A
    vector<vector<double>> temp(ROW,vector<double>(COL,0));
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            double sum=0;
            for(int k=0;k<COL;k++){
                sum+=DCTAndIDCTArray[k][i]*originMatrix[k][j];
            }
            temp[i][j]=sum;
        }
    }
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            double sum=0;
            for(int k=0;k<COL;k++){
                sum+=temp[i][k]*DCTAndIDCTArray[k][j];
            }
            originMatrix[i][j]=sum;
        }
    }
}

void JPEGData::deQuality(double** originMatrix,int qualityID){
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            originMatrix[i][j]*=quality[qualityID].table[i*ROW+j];
        }
    }
}

void JPEGData::Quality(double* originMatrix,int qualityID){
    for(int i=0;i<ROW*COL;i++){
        originMatrix[i]=round(originMatrix[i]/(qualityID==1?YQualityTable[i]:CQualityTable[i]));
    }
}

void JPEGData::PAndNCorrect(double** originMatrix){
    for(int i=0;i<ROW;i++)
        if(i%2==1)
            for(int j=0;j<COL;j++) 
                originMatrix[i][j]=-originMatrix[i][j];
}

string JPEGData::FlagCheck(fstream& file,int byteInfo){
    if(byteInfo==0xff){
        uint8_t info=file.get();
        string res=bitset<8>(0xFF).to_string();
        if(info==0xD9) {EOI=true;return "false";}
        else if(info==0x00) return res;
        return res + bitset<8>(info).to_string();
    }
    return bitset<8>(byteInfo).to_string();
}

uint16_t JPEGData::ReadByte(fstream& file,int len){
    uint16_t res=file.get();
    if(len!=1){
        res=(res<<8)+(uint8_t)file.get();
    }
    return res;
}

uint16_t JPEGData::findHuffmanCodeByBit(fstream& file,int& length,int& pos,string& deque,int curValue,int& curValLen){
    if(pos==length&&length>=HUFFMAN_DECODE_DEQUE_CACHE){//达到最大缓存
        deque = deque.substr(pos);
        int info=file.get();
        string res=FlagCheck(file,info);
        string str=bitset<8>(info).to_string();
        if(res=="false") res=bitset<8>(file.get()).to_string();
        deque.append(res);
        length = deque.length();
        pos = 0;
    }
    else if(length==0 || pos>=length){
        if(length==0){
            deque="";
            pos=0;
        }
        int info=file.get();
        string res=FlagCheck(file,info);
        string str=bitset<8>(info).to_string();
        if(res=="false") res=bitset<8>(file.get()).to_string();
        deque.append(res);
        length+=8;
    }
    curValue = (curValue << 1) + (uint8_t)(deque.at(pos++) - '0');
    curValLen++;
    return curValue;
}

void JPEGData::createDCEnHuffman(){
    JPEGHuffmanEncode table1,table2;
    int curCode=0,curPos=0;
    for(int i=0;i<16;i++){
        int count=bits_dc_luminance[i];
        curCode<<=1;
        while(count--){
            uint16_t code=curCode++;
            uint8_t bit=i+1,weight=val_dc_luminance[curPos++];
            pair<uint16_t, uint8_t> t1(code,bit);
            pair<uint8_t, pair<uint16_t, uint8_t>> t2(weight,t1);
            table1.table.insert(t2);
        }
    }
    curCode=curPos=0;
    for(int i=0;i<16;i++){
        int count=bits_dc_chrominance[i];
        curCode<<=1;
        while(count--){
            uint16_t code=curCode++;
            uint8_t bit=i+1,weight=val_dc_chrominance[curPos++];
            pair<uint16_t, uint8_t> t1(code,bit);
            pair<uint8_t, pair<uint16_t, uint8_t>> t2(weight,t1);
            table2.table.insert(t2);
        }
    }
    en_dc_huffman.push_back(table1);
    en_dc_huffman.push_back(table2);
}
void JPEGData::createACEnHuffman(){
    JPEGHuffmanEncode table1,table2;
    int curCode=0,curPos=0;
    for(int i=0;i<16;i++){
        int count=bits_ac_luminance[i];
        curCode<<=1;
        while(count--){
            uint16_t code=curCode++;
            uint8_t bit=i+1,weight=val_ac_luminance[curPos++];
            pair<uint16_t, uint8_t> t1(code,bit);
            pair<uint8_t, pair<uint16_t, uint8_t>> t2(weight,t1);
            table1.table.insert(t2);
        }
    }
    curCode=curPos=0;
    for(int i=0;i<16;i++){
        int count=bits_ac_chrominance[i];
        curCode<<=1;
        while(count--){
            uint16_t code=curCode++;
            uint8_t bit=i+1,weight=val_ac_chrominance[curPos++];
            pair<uint16_t, uint8_t> t1(code,bit);
            pair<uint8_t, pair<uint16_t, uint8_t>> t2(weight,t1);
            table2.table.insert(t2);
        }
    }
    en_ac_huffman.push_back(table1);
    en_ac_huffman.push_back(table2);
}

void BMPData::readBMP(const string& filePath){
    fstream file(filePath.c_str(),ios::in|ios::binary);
    if(file.fail()) return;
    uint8_t head[54]="";
    for(int i=0;i<54;i++) head[i]=file.get();
}

void BMPData::Init(){
    width+=4-width%4;
    rowSize = ceil((gray?8:24) * width / 32) * 4;
    dataSize = rowSize * height + 54 + (gray?4*256:0);
    bitmap=new uint8_t[dataSize];
    offbits = 54 + (gray ? 4 * 256 : 0);
    SetBitmapInfo(dataSize,height,width);
    if(gray) {
        BmpHeader[28]=0x08;
        // fill the header area
        for (int i = 0; i < 54; i++) bitmap[i] = BmpHeader[i];
        // init Palette
        for(int i=0;i<256;i++){
            bitmap[i*4+0+54]=i;
            bitmap[i*4+1+54]=i;
            bitmap[i*4+2+54]=i;
            bitmap[i*4+3+54]=0;
        }
    }
    for(int i=0;i<54;i++){
        bitmap[i]=BmpHeader[i];
    }
}

void BMPData::GrayEncoder(double (*convert)(double), double (*GrayAlgorithm)(RGB)){
    // fill the data area
	for (int i = 0; i < height; i++)
	{
		// compute the offset of destination bitmap and source image
		int idx = height - 1 - i,j=0;
		int offsetDst = idx * rowSize + 54 + (gray?4*256:0); // 54 means the header length
		// fill data
		for (j = 0; j < width; j++)
		{
			grayBuf->setValue(i,j,(uint8_t)convert(GrayAlgorithm(buf.getValue(i,j))));
		}
		// fill 0x0, this part can be ignored
		for (; j < rowSize; j++)
		{
			grayBuf->setValue(i,j,0x0);
		}
	}
}

void BMPData::GaussianHandle(bool isRGB, double (*convert)(double), int flag){
	//高斯变换矩阵
	double** gaussian=getTwoDimGaussianDistrbute(GAUSSIAN_RADIUS,GAUSSIAN_TEMPLATE_RADIUS);
    if(!isRGB){
        Matrix<uint8_t> *temp=new Matrix<uint8_t>(height,width);
        for(int i=0;i<height;i++){
            int offsetDst = (height-1-i) * rowSize + 54 + (gray?4*256:0);
            for(int j=0;j<rowSize;j++){
                double sum = 0;
                for(int x = i - GAUSSIAN_TEMPLATE_RADIUS; x <= i + GAUSSIAN_TEMPLATE_RADIUS; x++){
                    for(int y = j - GAUSSIAN_TEMPLATE_RADIUS; y <= j + GAUSSIAN_TEMPLATE_RADIUS; y++){
                        if(x<0||y<0||y>=width||x>=height) continue;
                        int row = x + GAUSSIAN_TEMPLATE_RADIUS - i,
                            col = y + GAUSSIAN_TEMPLATE_RADIUS - j;
                        sum += (grayBuf->getValue(x,y) * gaussian[row][col]);
                    }
                }
                temp->setValue(i,j,(uint8_t)convert(sum));
            }
        }
        delete grayBuf;
        grayBuf=temp;
    }
    else{
        Matrix<RGB> *temp=new Matrix<RGB>(height,width);
        for (int i = 0; i < height; i++){
            int offsetDst = (height - 1 - i) * rowSize + 54 + (gray?4*256:0);
            for (int j = 0; j < width; j++){
                double sum[3]={0};
                for(int x = i - GAUSSIAN_TEMPLATE_RADIUS; x <= i + GAUSSIAN_TEMPLATE_RADIUS; x++){
                    for(int y = j - GAUSSIAN_TEMPLATE_RADIUS; y <= j + GAUSSIAN_TEMPLATE_RADIUS; y++){
                        if(x<0||y<0||y>=width||x>=height) continue;
                        int row = x + GAUSSIAN_TEMPLATE_RADIUS - i,
                            col = y + GAUSSIAN_TEMPLATE_RADIUS - j;
                        sum[0] += (buf.getValue(x,y).blue * gaussian[row][col]);
                        sum[1] += (buf.getValue(x,y).green * gaussian[row][col]);
                        sum[2] += (buf.getValue(x,y).red * gaussian[row][col]);
                    }
                }
                RGB t;
                if(flag==0||flag==1) t.blue=convert(sum[0]);
                if(flag==0||flag==2) t.green=convert(sum[1]);
                if(flag==0||flag==3) t.red=convert(sum[2]);
                temp->setValue(i,j,t);
            }
        }
        buf.~Matrix();
        buf=*temp;
    }
	FREE_LP_2(gaussian,2*GAUSSIAN_TEMPLATE_RADIUS+1)
}

void BMPData::EdgeDetect(double matrix1[3][3],double matrix2[3][3],int row,uint8_t (*algorithm)(double,double)){
    Matrix<uint8_t> *temp=new Matrix<uint8_t>(height,width);
    for(int i=1;i<height-1;i++){
        for(int j=1;j<width-1;j++){
            double g_x=0,g_y=0;
            for(int x=0;x<row;x++){
                for(int y=0;y<row;y++){
                    g_x+=matrix1[x][y]*grayBuf->getValue(i+x-1,j+y-1);
                    g_y+=matrix2[x][y]*grayBuf->getValue(i+x-1,j+y-1);
                }
            }
            // g_x+=grayBuf->getValue(i-1,j-1)+grayBuf->getValue(i-1,j)+grayBuf->getValue(i-1,j+1)
            //     -grayBuf->getValue(i+1,j-1)-grayBuf->getValue(i+1,j)-grayBuf->getValue(i+1,j+1);
            // g_y+=grayBuf->getValue(i-1,j+1)+grayBuf->getValue(i,j+1)+grayBuf->getValue(i+1,j+1)
            //     -grayBuf->getValue(i-1,j-1)-grayBuf->getValue(i,j-1)-grayBuf->getValue(i+1,j-1);
            //如果加入此行，因为之前已经灰度化了，所以会出现灰色
            // g_x/=3,g_y/=3;
            temp->setValue(i,j,algorithm(g_x,g_y));
        }
    }
    delete grayBuf;
    grayBuf=temp;
}

void BMPData::saveBMP(const char *fileName){
    if(gray){
        // fill the data area
        for (int i = 0; i < height; i++)
        {
            // compute the offset of destination bitmap and source image
            int idx = height - 1 - i,j=0;
            int offsetDst = idx * rowSize + 54 + (gray?4*256:0); // 54 means the header length
            // fill data
            for (j = 0; j < width; j++)
            {
                bitmap[offsetDst+j]=grayBuf->getValue(i,j);
            }
            // fill 0x0, this part can be ignored
            for (; j < rowSize; j++)
            {
                bitmap[offsetDst +j] = 0x0;
            }
        }
    }
    else{
        int i=0;
        // fill the data area
        for (i = 0; i < height; i++)
        {
            // compute the offset of destination bitmap and source image
            int idx = height - 1 - i,j = 0;
            int offsetDst = idx * rowSize + 54 + (gray?4*256:0); // 54 means the header length
            // fill data
            for (j = 0; j < width*3; j++)
            {
                RGB temp=buf.getValue(i,j/3);
                if(j%3==0&&(flag==0||flag==1)) bitmap[offsetDst + j] = temp.blue;
                else if(j%3==1&&(flag==0||flag==2)) bitmap[offsetDst + j] = temp.green;
                else if(j%3==2&&(flag==0||flag==3)) bitmap[offsetDst + j] = temp.red;
            }
            // fill 0x0, this part can be ignored
            for (; j < rowSize; j++)
            {
                bitmap[offsetDst +j] = 0x0;
            }
        }
    }
	FILE *fp = fopen(fileName, "wb+");
	fwrite(bitmap, 1, dataSize, fp);
	fclose(fp);
}

// RGB BMPData::getRGB(int mcu_height,int mcu_width,int row,int col){
// 	int offsetHeight = (int)floor(row*1.0/mcu_height)*(int)ceil(width*1.0/mcu_width);
// 	int pos=col/mcu_width+offsetHeight;
// 	if(pos>=buf.size()) pos=buf.size()-1;
// 	return buf[pos][row%mcu_height][col%mcu_height];
// }

void BMPData::SetBitmapInfo(int dataSize,int height,int width){
	for (int i = 0; i < 4; i++)
	{
		// size of image ( header + data )
		BmpHeader[2 + i] = dataSize & 0xff;
		dataSize >>= 8;

		// width of image
		BmpHeader[18 + i] = width & 0xff;
		width >>= 8;

		// height of image
		BmpHeader[22 + i] = height & 0xff;
		height >>= 8;

        // offset of image data
        BmpHeader[10 + i] = offbits & 0xff;
        offbits >>= 8;
    }
}

void BMPData::analysisHead(uint8_t *head){
    for(int i=3;i>=0;i--){
        // size of image ( all data )
        dataSize<<=8;
        dataSize+=head[2+i];
        // offset of image data
        offbits<<=8;
        offbits+=head[0xa +i];
        // width of image
        width<<=8;
        width+=head[18+i];
        // height of image
        height<<=8;
        height+=head[22+i];
    }
    
}


NAME_SPACE_END()
