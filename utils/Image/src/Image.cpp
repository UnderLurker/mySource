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

uint16_t findHuffmanCodeByBit(fstream& file,int& length,int& pos,string& deque,int curValue,int& curValLen){
    if(length==0 || pos==length){
        if(length==0){
            deque="";
            pos=0;
        }
        deque.append(bitset<8>(file.get()).to_string());
        length+=8;
    }
    else if(length>=HUFFMAN_DECODE_DEQUE_CACHE){//达到最大缓存
        deque = deque.substr(pos);
        deque.append(bitset<8>(file.get()).to_string());
        length = deque.length();
        pos = 0;
    }
    curValue = (curValue << 1) + (uint8_t)(deque.at(pos++) - '0');
    curValLen++;
    return curValue;
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
        cout<<endl;
        while(len--){
            int info=file.get();
            temp.push_back(info);
            cout<<hex<<info<<" ";
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
        while(1){
            for(int i=0;i<3;i++){
                for(int j=0;j<YUV[i];j++){
                    int *matrix=new int[64];
                    int valCount=0;
                    uint8_t dcID = scan.componentHuffmanMap[component[i].colorId].first;
                    uint8_t acID = scan.componentHuffmanMap[component[i].colorId].second;
                    while(valCount<64){
                        if(valCount==0){//直流分量
                            //用curBitDeque和curBit去找权重
                            JPEGHuffmanCode::iterator it;
                            while(dc_huffman[dcID].findKey(curValue,curValueLength,it)){
                                curValue=findHuffmanCodeByBit(file,curBitDequeLength,curBitPos,curBitDeque,curValue,curValueLength);
                            }
                            uint8_t weight = it->second.second;
                            curValue=0;//这里变为dc值
                            curValueLength=0;
                            while(weight--){
                                curValue=findHuffmanCodeByBit(file,curBitDequeLength,curBitPos,curBitDeque,curValue,curValueLength);
                            }
                            preDCValue+=curValue;
                        }
                        else{//交流分量
                            JPEGHuffmanCode::iterator it;
                            while(dc_huffman[acID].findKey(curValue,curValueLength,it)){
                                curValue=findHuffmanCodeByBit(file,curBitDequeLength,curBitPos,curBitDeque,curValue,curValueLength);
                            }
                            uint8_t weight = it->second.second & 0x0f;
                            uint8_t zeroCount = it->second.second >> 4;
                            curValue=0;//这里变为ac值
                            curValueLength=0;
                            while(weight--){
                                curValue=findHuffmanCodeByBit(file,curBitDequeLength,curBitPos,curBitDeque,curValue,curValueLength);
                            }
                            while(zeroCount--){
                                matrix[valCount++]=0;
                            }
                        }
                        matrix[valCount++]=curValue;
                        curValue=0;
                        curValueLength=0;
                    }
                    cout<<endl;
                    for(int i=0;i<64;i++){
                        cout.width(3);
                        cout.fill('0');
                        cout<<matrix[i]<<" ";
                    }
                    deHuffman.push_back(matrix);
                }
            }
            curMCUCount++;
            cout<<curMCUCount;
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