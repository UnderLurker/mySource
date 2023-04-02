#include "Image.h"
#include "Util.h"
#include <algorithm>
#include <exception>
#include <fstream>
#include <stdint.h>
#include <bitset>
#include <utility>

NAME_SPACE_START(myUtil)

uint16_t ReadByte(fstream& file,int len){
    uint16_t res=file.get();
    if(len!=1){
        res=(res<<8)+(uint8_t)file.get();
    }
    return res;
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
                pair<uint16_t, uint8_t> t1(curCode,i+1);
                pair<pair<uint16_t, uint8_t>, uint8_t> t2(t1,temp[curPos]);
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
                    while(count--) file.get();
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
        flag=deQuantity();
        if(!flag) return false;
        //反zig-zag排序
        flag=deZSort();
        if(!flag) return false;
        //反离散余弦变换

        if(!flag) return false;
        //YCbCr转RGB

        if(!flag) return false;
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
        int YCount=component[0].h_samp_factor*component[0].v_samp_factor;
        int UCount=component[1].h_samp_factor*component[1].v_samp_factor;
        int VCount=component[2].h_samp_factor*component[2].v_samp_factor;
        int YUV[]={YCount,UCount,VCount};
        int curMCUCount=0;//当前MCU数量，为了重置直流分量
        int preDCValue=0;//上一个DC分量
        int curBit=0;//当前值有多少位
        int curValue=0;//当前的值
        string curBitDeque="";//用来存储读出来的2进制数
        while(1){
            //一次循环解析一个MCU
            for(int i=0;i<3;i++){
                for(int j=0;j<YUV[i];j++){
                    int valCount=0;
                    uint8_t dcID = scan.componentHuffmanMap[component[i].colorId].first;
                    uint8_t acID = scan.componentHuffmanMap[component[i].colorId].second;
                    while(valCount<64){
                        if(valCount==0){//直流分量
                            //用curBitDeque和curBit去找值
                            //preDCValue=
                        }
                        else{//交流分量

                        }
                    }
                }
            }
            curMCUCount++;
            if(curMCUCount==resetInterval) preDCValue=0;
        }
    } catch (...) {
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

NAME_SPACE_END()