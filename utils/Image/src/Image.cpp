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
            int cbPos = YUV[0] + (int)((k / ROW) * cb_v_samp_scale) + (int)((j / COL) * cb_h_samp_scale);
            int crPos = YUV[0] + YUV[1] + (int)((k / ROW) * cr_v_samp_scale) + (int)((j / COL) * cr_h_samp_scale);
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

Matrix<RGB> JPEGData::getRGBMatrix(){
    Matrix<RGB> res(height,width);
    int mcu_height=8*max_v_samp_factor,
        mcu_width=8*max_h_samp_factor;
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

void JPEGData::PAndNCorrect(double** originMatrix){
    for(int i=0;i<ROW;i++)
        if(i%2==1)
            for(int j=0;j<COL;j++) 
                originMatrix[i][j]=-originMatrix[i][j];
}

string JPEGData::FlagCkeck(fstream& file,int byteInfo){
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
        string res=FlagCkeck(file,info);
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
        string res=FlagCkeck(file,info);
        string str=bitset<8>(info).to_string();
        if(res=="false") res=bitset<8>(file.get()).to_string();
        deque.append(res);
        length+=8;
    }
    curValue = (curValue << 1) + (uint8_t)(deque.at(pos++) - '0');
    curValLen++;
    return curValue;
}


void BMPData::Init(){
    rowSize = ceil((gray?8:24) * width / 32) * 4;
    dataSize = rowSize * height + 54 + (gray?4*256:0);
    bitmap=new uint8_t[dataSize];
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

void BMPData::EdgeDetectPrewitt(){
    Matrix<uint8_t> *temp=new Matrix<uint8_t>(height,width);
    for(int i=1;i<height-1;i++){
        for(int j=1;j<width-1;j++){
            double g_x=0,g_y=0;
            g_x+=grayBuf->getValue(i-1,j-1)+grayBuf->getValue(i-1,j)+grayBuf->getValue(i-1,j+1)
                -grayBuf->getValue(i+1,j-1)-grayBuf->getValue(i+1,j)-grayBuf->getValue(i+1,j+1);
            g_y+=grayBuf->getValue(i-1,j+1)+grayBuf->getValue(i,j+1)+grayBuf->getValue(i+1,j+1)
                -grayBuf->getValue(i-1,j-1)-grayBuf->getValue(i,j-1)-grayBuf->getValue(i+1,j-1);
            g_x/=3,g_y/=3;
            temp->setValue(i,j,max(abs(g_x),abs(g_y)));
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
        // fill the data area
        for (int i = 0; i < height; i++)
        {
            // compute the offset of destination bitmap and source image
            int idx = height - 1 - i;
            int offsetDst = idx * rowSize + 54 + (gray?4*256:0); // 54 means the header length
            // fill data
            for (int j = 0; j < width*3; j++)
            {
                if(j%3==0&&(flag==0||flag==1)) bitmap[offsetDst + j] = buf.getValue(i,j/3).blue;
                else if(j%3==1&&(flag==0||flag==2)) bitmap[offsetDst + j] = buf.getValue(i,j/3).green;
                else if(j%3==2&&(flag==0||flag==3)) bitmap[offsetDst + j] = buf.getValue(i,j/3).red;
            }
            // fill 0x0, this part can be ignored
            for (int j = width*3; j < rowSize; j++)
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
		BmpHeader[22  + i] = height & 0xff;
		height >>= 8;
	}
}


NAME_SPACE_END()
