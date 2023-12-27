//
// Created by 常笑男 on 2023/12/28.
//
#include "jpeg.h"
#include "ImageUtil.h"
#include "ImageConst.h"
#include <vector>

using namespace std;

NAME_SPACE_START(myUtil)


int32_t RGBValueLimit(double input)
{
    if (input < 0)
        return 0;
    else if (input > 255)
        return 255;
    // 四舍五入、取整均可
    // return (int)(input);
    return static_cast<int32_t>(round(input));
}

double YCbCrValueLimit(double input)
{
    if (input < -128)
        return -128;
    else if (input > 128)
        return 128;
    return input;
}

int32_t getBitLength(int32_t num)
{
    int32_t res = 0;
    while (num) {
        res += 1;
        num >>= 1;
    }
    return res;
}

double** UnZigZag(const int32_t* originArray, uint32_t length)
{
    auto** table = new double*[ROW];
    for (int32_t i = 0; i < ROW; i++)
        table[i] = new double[COL];
    int32_t cur = 0, x = 0, y = 0;
    bool flag = true; // true是右上 false是左下
    while (cur < length) {
        table[y][x] = originArray[cur++];
        if (flag) {
            x++;
            y--;
        } else {
            x--;
            y++;
        }
        if (x < 0 || y < 0 || x > 7 || y > 7)
            flag = !flag;
        if (x < 0 && y > 7) {
            x = 1;
            y = 7;
        }
        if (x < 0)
            x = 0;
        else if (x > 7) {
            x = 7;
            y += 2;
        }
        if (y < 0)
            y = 0;
        else if (y > 7) {
            y = 7;
            x += 2;
        }
    }
    return table;
}

double* ZigZag(double** originArray, uint32_t row, uint32_t col)
{
    auto* res = new double[row * col];
    int32_t cur = 0, x = 0, y = 0;
    bool flag = true; // true是右上 false是左下
    while (cur < row * col) {
        res[cur++] = round(originArray[y][x]);
        if (flag) {
            x++;
            y--;
        } else {
            x--;
            y++;
        }
        if (x < 0 || y < 0 || x > 7 || y > 7)
            flag = !flag;
        if (x < 0 && y > 7) {
            x = 1;
            y = 7;
        }
        if (x < 0)
            x = 0;
        else if (x > 7) {
            x = 7;
            y += 2;
        }
        if (y < 0)
            y = 0;
        else if (y > 7) {
            y = 7;
            x += 2;
        }
    }
    return res;
}

void writeByte(fstream& file, uint8_t val)
{
    file.write((char*)&val, 1);
}

void writeTwoByte(fstream& file, uint16_t val)
{
    writeByte(file, val >> 8);
    writeByte(file, val & 0xFF);
}

bool JPEGScan::Init(fstream& file, uint16_t len)
{
    try {
        uint8_t count = file.get();
        len--;
        while (count--) {
            uint8_t componentId = file.get();
            uint8_t table = file.get();
            uint8_t dcId = table >> 4;
            uint8_t acId = table & 0x0f;
            pair<uint8_t, uint8_t> info1(dcId, acId);
            pair<uint8_t, pair<uint8_t, uint8_t>> info2(componentId, info1);
            componentHuffmanMap.insert(info2);
        }
    } catch (...) {
        return false;
    }
    return true;
}

bool JPEGHuffmanCode::Init(fstream& file, uint16_t len)
{
    try {
        vector<uint8_t> temp;
        while (len--) {
            int32_t info = file.get();
            temp.push_back(info);
        }
        int32_t curPos = 16, curCode = 0;
        for (int32_t i = 0; i < 16; i++) {
            int32_t count = temp[i];
            curCode <<= 1;
            while (count--) {
                uint16_t code = curCode;
                uint8_t bit = i + 1;
                uint8_t weight = temp[curPos];
                pair<uint8_t, uint8_t> t1(bit, weight);
                pair<uint16_t, pair<uint8_t, uint8_t>> t2(curCode, t1);
                table.insert(t2);
                curCode++;
                curPos++;
            }
        }
    } catch (...) {
        return false;
    }
    return true;
}

bool JPEGHuffmanCode::findKey(const uint16_t& code, const uint8_t& bit, iterator& it)
{
    it = table.find(code);
    if (it == table.end())
        return true;
    return it->second.first != bit;
}

void JPEGHuffmanCode::write(fstream& file)
{
    // Y dc
    writeByte(file, (uint8_t)FLAG);
    writeByte(file, (uint8_t)DHT);
    writeTwoByte(file, (uint16_t)0x001F);
    writeByte(file, (uint8_t)0x00);
    for (auto i : bits_dc_luminance)
        writeByte(file, i);
    for (auto i : val_dc_luminance)
        writeByte(file, i);
    // Y ac
    writeByte(file, (uint8_t)FLAG);
    writeByte(file, (uint8_t)DHT);
    writeTwoByte(file, (uint16_t)0x00B5);
    writeByte(file, (uint8_t)0x10);
    for (auto i : bits_ac_luminance)
        writeByte(file, i);
    for (auto i : val_ac_luminance)
        writeByte(file, i);
    // UV dc
    writeByte(file, (uint8_t)FLAG);
    writeByte(file, (uint8_t)DHT);
    writeTwoByte(file, (uint16_t)0x001F);
    writeByte(file, (uint8_t)0x01);
    for (auto i : bits_dc_chrominance)
        writeByte(file, i);
    for (auto i : val_dc_chrominance)
        writeByte(file, i);
    // UV ac
    writeByte(file, (uint8_t)FLAG);
    writeByte(file, (uint8_t)DHT);
    writeTwoByte(file, (uint16_t)0x00B5);
    writeByte(file, (uint8_t)0x11);
    for (auto i : bits_ac_chrominance)
        writeByte(file, i);
    for (auto i : val_ac_chrominance)
        writeByte(file, i);
}

bool JPEGQuality::Init(fstream& file, uint16_t len)
{
    try {
        int32_t info = file.get();
        precision = info >> 4;
        id = info & 0x0f;
        len--;
        while (len--) {
            int32_t t = file.get();
            table.push_back(t);
        }
    } catch (...) {
        return false;
    }
    return true;
}

void JPEGQuality::write(fstream& file)
{
    writeByte(file, FLAG);
    writeByte(file, DQT);
    writeTwoByte(file, (uint16_t)0x0043);
    writeByte(file, (uint8_t)0x00);
    for (auto i : YQualityTable) {
        writeByte(file, i);
    }
    writeByte(file, FLAG);
    writeByte(file, DQT);
    writeTwoByte(file, (uint16_t)0x0043);
    writeByte(file, (uint8_t)0x01);
    for (auto i : CQualityTable) {
        writeByte(file, i);
    }
}

bool JPEGComponent::Init(fstream& file, uint16_t len)
{
    try {
        int32_t info1 = file.get();
        int32_t info2 = file.get();
        int32_t info3 = file.get();
        colorId = info1;
        h_samp_factor = info2 >> 4;
        v_samp_factor = info2 & 0x0f;
        qualityId = info3;
    } catch (...) {
        return false;
    }
    return true;
}

bool JPEGData::read(const char* filePath)
{
    fstream file(filePath, ios::in | ios::binary);
    if (file.fail())
        return false;
    file.seekg(0, ios::end);
    pos = file.tellg();
    file.seekg(2, ios::beg);
    dc_huffman.resize(2);
    ac_huffman.resize(2);
    try {
        // do read data through using other method
        uint16_t pLen = 0;
        uint16_t pMarker = 0xFF;
        uint16_t pType = 0x00;
        while (!file.eof()) {
            pMarker = file.get();
            pType = file.get();
            if (pType == EOI)
                break;

            pLen = file.get();
            pLen = (pLen << 8) + file.get();
            if (pMarker != 0xFF)
                throw exception();
            bool flag = encodeProcess(file, pLen, pType);
            if (!flag)
                throw exception();
        }
    } catch (...) {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool JPEGData::writeJPEG(const char* filePath, int32_t samp_factor[3][2], int32_t quality_scale)
{
    auto _rgb = getRGBMatrix();
    max_h_samp_factor = 0;
    max_v_samp_factor = 0;
    for (int32_t i = 0; i < 3; i++) {
        write_samp_factor[i][0] = samp_factor[i][0];
        write_samp_factor[i][1] = samp_factor[i][1];
        max_h_samp_factor = max(max_h_samp_factor, write_samp_factor[i][0]);
        max_v_samp_factor = max(max_v_samp_factor, write_samp_factor[i][1]);
    }
    initQualityTable(quality_scale);
    fstream file(filePath, ios::out | ios::binary);
    if (file.fail())
        return false;
    try {
        createDCEnHuffman();
        createACEnHuffman();
        writeTwoByte(file, (uint16_t)((FLAG << 8) + SOI));  // SOI
        for (auto i : APP) {                                // APP
            writeByte(file, i);
        }
        JPEGQuality::write(file);                               // DQT
        writeTwoByte(file, (uint16_t)((FLAG << 8) + SOF0)); // SOF
        writeTwoByte(file, (uint16_t)0x0011);
        writeByte(file, (uint8_t)0x08);
        writeTwoByte(file, (uint16_t)height);
        writeTwoByte(file, (uint16_t)width);
        writeByte(file, (uint8_t)0x03);

        writeByte(file, (uint8_t)0x01);
        writeByte(file, (uint8_t)((samp_factor[0][0] << 4) | samp_factor[0][1]));
        writeByte(file, (uint8_t)0x00);

        writeByte(file, (uint8_t)0x02);
        writeByte(file, (uint8_t)((samp_factor[1][0] << 4) | samp_factor[1][1]));
        writeByte(file, (uint8_t)0x01);

        writeByte(file, (uint8_t)0x03);
        writeByte(file, (uint8_t)((samp_factor[2][0] << 4) | samp_factor[2][1]));
        writeByte(file, (uint8_t)0x01);

        JPEGHuffmanCode::write(file);                      // DHT
        writeTwoByte(file, (uint16_t)((FLAG << 8) + DRI)); // DRI
        writeTwoByte(file, (uint16_t)0x0004);
        writeTwoByte(file, (uint16_t)ceil(_rgb.col * 1.0 / (ROW * max_h_samp_factor)));
        writeTwoByte(file, (uint16_t)((FLAG << 8) + SOS)); // SOS
        writeTwoByte(file, (uint16_t)0x000C);
        writeByte(file, (uint8_t)0x03);

        writeByte(file, (uint8_t)0x01);
        writeByte(file, (uint8_t)0x00);

        writeByte(file, (uint8_t)0x02);
        writeByte(file, (uint8_t)0x11);

        writeByte(file, (uint8_t)0x03);
        writeByte(file, (uint8_t)0x11);

        writeByte(file, (uint8_t)0);
        writeByte(file, (uint8_t)0x3F);
        writeByte(file, (uint8_t)0);
        RGBToYCbCr(_rgb, file);
        writeTwoByte(file, (uint16_t)((FLAG << 8) + (uint8_t)JPEGPType::EOI)); // EOI
    } catch (...) {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool JPEGData::readSOF(fstream& file, uint16_t len)
{
    try {
        precision = file.get();
        height = max(height, (int32_t)ReadByte(file, 2));
        width = max(width, (int32_t)ReadByte(file, 2));
        int32_t count = ReadByte(file, 1);
        if (count != 3)
            return false;
        len -= 6;
        component.resize(count);
        for (int32_t i = 0; i < count; i++) {
            JPEGComponent com {};
            com.Init(file, len / 3);
            max_h_samp_factor = max(max_h_samp_factor, (int32_t)com.h_samp_factor);
            max_v_samp_factor = max(max_v_samp_factor, (int32_t)com.v_samp_factor);
            component[i] = com;
        }
        if ((component[0].h_samp_factor * component[0].v_samp_factor) /
                (component[1].h_samp_factor * component[1].v_samp_factor) == 4) {
            isYUV411 = true;
        } else if ((component[0].h_samp_factor * component[0].v_samp_factor) /
                       (component[1].h_samp_factor * component[1].v_samp_factor) == 2) {
            isYUV422 = true;
        } else if ((component[0].h_samp_factor * component[0].v_samp_factor) /
                       (component[1].h_samp_factor * component[1].v_samp_factor) == 1) {
            isYUV111 = true;
        }
        mcu_rows = ceil(height / (8 * max_v_samp_factor));
        mcu_cols = ceil(width / (8 * max_h_samp_factor));
    } catch (...) {
        return false;
    }
    return true;
}

bool JPEGData::readData(fstream& file)
{
    bool flag = true;
    try {
        // 使用huffman表来解出RLE编码，接着转回长度为64的矩阵
        flag = huffmanDecode(file);
        if (!flag)
            return false;
    } catch (...) {
        return false;
    }
    return true;
}

bool JPEGData::huffmanDecode(fstream& file)
{
    try {
        // 原图像一个MCU有多少8*8矩阵（此时是YCbCr还没有分开）
        // int MCUBlockCount=max_h_samp_factor*max_v_samp_factor;
        // 顺序YCbCr
        int32_t YUV[] = { component[0].h_samp_factor * component[0].v_samp_factor,
            component[1].h_samp_factor * component[1].v_samp_factor,
            component[2].h_samp_factor * component[2].v_samp_factor };
        int32_t curMCUCount = 1;    // 当前MCU数量
        int32_t curValueLength = 0; // 当前值有多少位
        int32_t curValue = 0;       // 当前的值

        int32_t curBitDequeLength = 8;   // 当前curBitDeque长度
        int32_t curBitPos = 0;           // 当前string读取到第几位
        int32_t restart = resetInterval; // 直流分量重置
        string curBitDeque;     // 用来存储读出来的2进制数
        curBitDeque.append(bitset<8>(file.get()).to_string());
        curBitDequeLength = 8;
        while (!EOI || (pos - file.tellg()) != 2) { // 一次循环解析一个MCU
            int32_t count = 1;
            for (int32_t i = 0; i < 3; i++) {
                for (int32_t j = 0; j < YUV[i]; j++) {
                    int32_t matrix[64] = { 0 };
                    int32_t valCount = 0;
                    uint8_t dcID = scan.componentHuffmanMap[component[i].colorId].first;
                    uint8_t acID = scan.componentHuffmanMap[component[i].colorId].second;
                    int32_t qualityId = component[i].qualityId;
                    if (qualityId >= quality.size())
                        qualityId = 0;
                    while (valCount < 64) {
                        JPEGHuffmanCode::iterator it;   // 用curBitDeque和curBit去找权重，curValue作为当前键值
                        JPEGHuffmanCode& huffman = valCount == 0 ? dc_huffman[dcID] : ac_huffman[acID];
                        while (curValueLength <= 16 && huffman.findKey(curValue, curValueLength, it)) {
                            curValue = findHuffmanCodeByBit(
                                file, curBitDequeLength, curBitPos, curBitDeque, curValue, curValueLength);
                        }
                        if (curValueLength > 16)
                            return true;
                        uint8_t weight, zeroCount = 0;      // 已经找到了权重和位宽
                        if (valCount == 0)
                            weight = it->second.second;
                        else {
                            weight = it->second.second & 0x0f;
                            zeroCount = it->second.second >> 4;
                        }
                        curValue = 0; // 这里变为dc或ac值
                        curValueLength = 0;
                        if (valCount != 0 && weight == 0 && zeroCount == 0) // 后面全是0
                            break;
                        for (int32_t k = 0; k < weight; k++) {  // 读取真正的值
                            curValue = findHuffmanCodeByBit(
                                file, curBitDequeLength, curBitPos, curBitDeque, curValue, curValueLength);
                        }
                        if (curValue < pow(2.0, static_cast<double>(curValueLength - 1))) {
                            curValue = curValue - static_cast<int32_t>(pow(2, curValueLength)) + 1;
                        }
                        int32_t writeValue = valCount == 0 ? (preDCValue[i] += curValue) : curValue;
                        valCount += zeroCount;
                        writeValue *= quality[qualityId].table[valCount]; // 反量化
                        matrix[valCount] = writeValue;
                        curValue = 0;
                        curValueLength = 0;
                        valCount++;
                    }
                    double** tempZ = UnZigZag(matrix, ROW * COL); // 反zig-zag编码
                    // 反量化，在反zig-zag编码前后差别，前面：RGB数值与编辑器比偏小，反之偏大，这也与最后取整时的方式有关
                    //  deQuality(tempZ,qualityId);
                    //  print(tempZ);
                    // 隔行正负纠正，有的博客说了，但是没感觉有啥帮助
                    //  PAndNCorrect(tempZ);
                    IDCT(tempZ); // dct逆变换
                    ycbcr.push_back(tempZ);
                }
            }
            RGB** lpRGB = YCbCrToRGB(YUV);
            FREE_VECTOR_LP(ycbcr, ROW)
            rgb.push_back(lpRGB);
            // 直流分量重置间隔不为0的
            if (restart > 0) {
                resetInterval--;
                if (resetInterval == 0) {
                    resetInterval = restart;
                    curDRI += 1;
                    curDRI &= 0x7;
                    // 需要在此处读取两字节信息，看是否为重置标识
                    file.get();
                    if (file.get() == 0xD9)
                        EOI = true;
                    curBitPos = curBitDequeLength;
                    preDCValue[0] = 0;
                    preDCValue[1] = 0;
                    preDCValue[2] = 0;
                }
            }
            if (pos - file.tellg() == 2)
                break;
        }
        cout << "\nsuccessfully\n";
    } catch (exception ex) {
        cout << ex.what();
        return false;
    }
    return true;
}

RGB** JPEGData::YCbCrToRGB(const int32_t* YUV)
{
    RGB** res = new RGB*[ROW * max_v_samp_factor];
    int32_t matrixCount = YUV[0] + YUV[1] + YUV[2];
    int32_t crCount = 0, cbCount = 0;
    // 1＝Y, 2＝Cb, 3＝Cr
    // 式子 scale * x, scale * y
    double cb_h_samp_scale = component[1].h_samp_factor * 1.0 / max_h_samp_factor,
           cb_v_samp_scale = component[1].v_samp_factor * 1.0 / max_v_samp_factor,
           cr_h_samp_scale = component[2].h_samp_factor * 1.0 / max_h_samp_factor,
           cr_v_samp_scale = component[2].v_samp_factor * 1.0 / max_v_samp_factor;
    for (int32_t i = 0; i < ROW * max_v_samp_factor; i++)
        res[i] = new RGB[COL * max_h_samp_factor];
    // 此处直接生成rgb值
    // 注意，此处YCbCr的对应关系与采样因子有关
    for (int32_t j = 0; j < ROW * max_v_samp_factor; j++) {
        for (int32_t k = 0; k < COL * max_h_samp_factor; k++) {
            int32_t yPos = (j / ROW) * component[0].h_samp_factor + (k / COL);
            int32_t cbPos = YUV[0] + (int32_t)((k / COL) * cb_v_samp_scale) + (int32_t)((j / ROW) * cb_h_samp_scale);
            int32_t crPos = YUV[0] + YUV[1] + (int32_t)((k / COL) * cr_v_samp_scale) + (int32_t)((j / ROW) * cr_h_samp_scale);
            double y = ycbcr[yPos][j % ROW][k % COL];
            double cb = ycbcr[cbPos][(int32_t)(j * cb_v_samp_scale)][(int32_t)(k * cb_h_samp_scale)];
            double cr = ycbcr[crPos][(int32_t)(j * cr_v_samp_scale)][(int32_t)(k * cr_h_samp_scale)];

            res[j][k].red = RGBValueLimit(128 + y + 1.402 * cr);
            res[j][k].green = RGBValueLimit(128 + y - 0.71414 * cr - 0.34414 * cb);
            res[j][k].blue = RGBValueLimit(128 + y + 1.772 * cb);
        }
    }
    return res;
}

void JPEGData::RGBToYCbCr(Matrix<RGB> _rgb, fstream& file)
{
    int32_t mcu_width = COL * max_h_samp_factor, mcu_height = ROW * max_v_samp_factor;
    int32_t YUV[3] = { write_samp_factor[0][0] * write_samp_factor[0][1],
        write_samp_factor[1][0] * write_samp_factor[1][1],
        write_samp_factor[2][0] * write_samp_factor[2][1] };
    int32_t y_h_param = max_h_samp_factor * 1.0 / write_samp_factor[0][0],
            y_v_param = max_v_samp_factor * 1.0 / write_samp_factor[0][1],
            cb_h_param = max_h_samp_factor * 1.0 / write_samp_factor[1][0],
            cb_v_param = max_v_samp_factor * 1.0 / write_samp_factor[1][1],
            cr_h_param = max_h_samp_factor * 1.0 / write_samp_factor[2][0],
            cr_v_param = max_v_samp_factor * 1.0 / write_samp_factor[2][1];
    double cb_h_samp_scale = write_samp_factor[1][0] * 1.0 / max_h_samp_factor,
           cb_v_samp_scale = write_samp_factor[1][1] * 1.0 / max_v_samp_factor,
           cr_h_samp_scale = write_samp_factor[2][0] * 1.0 / max_h_samp_factor,
           cr_v_samp_scale = write_samp_factor[2][1] * 1.0 / max_v_samp_factor;
    preDCValue[0] = preDCValue[1] = preDCValue[2] = 0;
    mcu_rows = ceil(_rgb.row * 1.0 / mcu_height);
    mcu_cols = ceil(_rgb.col * 1.0 / mcu_width);
    int32_t DRI = mcu_cols, DriFLAG = 0xD0;
    for (int32_t mcu_y = 0; mcu_y < mcu_rows; mcu_y++) {
        for (int32_t mcu_x = 0; mcu_x < mcu_cols; mcu_x++) {
            auto*** yuv = new double**[YUV[0] + YUV[1] + YUV[2]];
            int32_t count = 0;
            for (int32_t i : YUV) {
                for (int32_t j = 0; j < i; j++) {
                    yuv[count] = new double*[ROW];
                    for (int32_t k = 0; k < ROW; k++) {
                        yuv[count][k] = new double[COL];
                        memset(yuv[count][k], 0, sizeof(double) * COL);
                    }
                    count++;
                }
            }
            int32_t row = mcu_y * mcu_height, col = mcu_x * mcu_width;
            for (int32_t i = 0; i < mcu_height; i++) {
                for (int32_t j = 0; j < mcu_width; j++) {
                    RGB t = _rgb.getValue(row + i, col + j); // 得到的是一整个mcu，但是要把它分成多个8*8矩阵
                    double y = YCbCrValueLimit(0.299 * t.red + 0.587 * t.green + 0.114 * t.blue - 128);
                    double cb = YCbCrValueLimit(-0.1687 * t.red - 0.3313 * t.green + 0.500 * t.blue);
                    double cr = YCbCrValueLimit(0.500 * t.red - 0.4187 * t.green - 0.0813 * t.blue);
                    int32_t yPos = (i / ROW) * max_h_samp_factor + (j / COL);
                    int32_t cbPos = YUV[0] + (int32_t)((j / COL) * cb_v_samp_scale) + (int32_t)((i / ROW) * cb_h_samp_scale);
                    int32_t crPos = YUV[0] + YUV[1] + (int32_t)((j / COL) * cr_v_samp_scale) +
                                    (int32_t)((i / ROW) * cr_h_samp_scale);

                    if (i % y_v_param == 0 && j % y_h_param == 0)
                        yuv[yPos][i % ROW][j % COL] = y;
                    if (i % cb_v_param == 0 && j % cb_h_param == 0)
                        yuv[cbPos][i / cb_v_param][j / cb_h_param] = cb;
                    if (i % cr_v_param == 0 && j % cr_h_param == 0)
                        yuv[crPos][i / cr_v_param][j / cr_h_param] = cr;
                }
            }
            int32_t pos = 0;
            for (int32_t i = 0; i < 3; i++) {
                int32_t huffmanID = i == 0 ? 0 : 1;
                for (int32_t j = 0; j < YUV[i]; j++, pos++) {
                    DCT(yuv[pos]);
                    auto* temp = ZigZag(yuv[pos], ROW, COL);
                    // encode DC
                    temp[0] = round(temp[0] / (i == 0 ? YQualityTable[0] : CQualityTable[0])); // quality
                    int32_t dcDiff = static_cast<int32_t>(temp[0]) - preDCValue[i];            // 进行差分编码
                    int32_t lenDC = getBitLength((int32_t)abs(dcDiff));
                    preDCValue[i] = static_cast<int32_t>(temp[0]);
                    if (dcDiff < 0)
                        dcDiff = static_cast<int32_t>(pow(2, lenDC)) + dcDiff - 1;
                    writeBit(file, lenDC, dcDiff, en_dc_huffman[huffmanID].table[lenDC]);
                    // encode AC
                    int32_t endPos = 63;
                    while (endPos > 0 && temp[endPos] == 0)
                        endPos--;
                    for (int32_t k = 1; k <= endPos; k++) {
                        int32_t zeroCount = 0;
                        temp[k] = round(temp[k] / (i == 0 ? YQualityTable[k] : CQualityTable[k])); // quality
                        while (k < endPos && temp[k] == 0) {
                            temp[k + 1] = round(temp[k + 1] / (i == 0 ? YQualityTable[k + 1] : CQualityTable[k + 1]));
                            zeroCount++;
                            k++;
                        }
                        int32_t lenAC = getBitLength((int32_t)abs(temp[k]));
                        if (temp[k] < 0)
                            temp[k] = pow(2, lenAC) + temp[k] - 1;
                        while (zeroCount >= 16) {
                            writeBitToFile(file, en_ac_huffman[huffmanID].table[0xf0].second,
                                en_ac_huffman[huffmanID].table[0xf0].first);
                            zeroCount -= 16;
                        }
                        writeBit(file, lenAC, static_cast<int32_t>(temp[k]),
                            en_ac_huffman[huffmanID].table[(zeroCount << 4) | lenAC]);
                        zeroCount = 0;
                    }
                    if (endPos != 63)
                        writeBitToFile(file, en_ac_huffman[huffmanID].table[0x00].second,
                            en_ac_huffman[huffmanID].table[0x00].first);
                }
            }
            FREE_LP_3(yuv, YUV[0] + YUV[1] + YUV[2], ROW)
        }
        if (bitCurPos != 0)
            writeByte(file, (uint8_t)curBitValue);
        bitCurPos = curBitValue = 0;
        preDCValue[0] = preDCValue[1] = preDCValue[2] = 0;
        writeTwoByte(file, (uint16_t)((FLAG << 8) + DriFLAG++));
        if (DriFLAG > 0xD7)
            DriFLAG = 0xD0;
    }
}

Matrix<RGB> JPEGData::getRGBMatrix()
{
    Matrix<RGB> res(height, width);
    int32_t mcu_height = ROW * max_v_samp_factor, mcu_width = COL * max_h_samp_factor;
    for (int32_t row = 0; row < height; row++) {
        int32_t rowOffset = (int32_t)floor(row * 1.0 / mcu_height) * (int32_t)ceil(width * 1.0 / mcu_width);
        for (int32_t col = 0; col < width; col++) {
            int32_t pos = rowOffset + col / (8 * max_h_samp_factor);
            if (pos >= rgb.size())
                pos = rgb.size() - 1;
            res.setValue(row, col, rgb[pos][row % mcu_height][col % mcu_width]);
        }
    }
    return res;
}

void JPEGData::writeBitToFile(fstream& file, int32_t len, int32_t realData)
{
    while (len > (8 - bitCurPos)) {
        int32_t rightMoveBit = len + bitCurPos - 8;
        int32_t bitValue = realData >> rightMoveBit;
        curBitValue |= bitValue;
        writeByte(file, (uint8_t)curBitValue);
        if (curBitValue == 0xFF)
            writeByte(file, (uint8_t)0);
        realData -= bitValue << rightMoveBit;
        len -= 8 - bitCurPos;
        curBitValue = bitCurPos = 0;
    }
    curBitValue |= realData << (8 - bitCurPos - len);
    bitCurPos += len;
    if (bitCurPos >= 8) {
        writeByte(file, (uint8_t)curBitValue);
        if (curBitValue == 0xFF)
            writeByte(file, (uint8_t)0);
        curBitValue = bitCurPos = 0;
    }
}
void JPEGData::writeBit(fstream& file, int32_t len, int32_t realData, pair<uint16_t, uint8_t>& huffmanCode)
{
    int32_t codeLen = huffmanCode.second, code = huffmanCode.first;
    writeBitToFile(file, codeLen, code);

    writeBitToFile(file, len, realData);
}

double** JPEGData::createDCTAndIDCTArray(int32_t row)
{
    auto** res = new double*[row];
    for (int32_t i = 0; i < row; i++)
        res[i] = new double[row];
    for (int32_t i = 0; i < row; i++) {
        for (int32_t j = 0; j < row; j++) {
            double t = 0;
            if (i == 0)
                t = sqrt(1.0 / row);
            else
                t = sqrt(2.0 / row);
            res[i][j] = t * cos(M_PI * (j + 0.5) * i / row);
        }
    }
    return res;
}

void JPEGData::DCT(double** originMatrix)
{
    // 原理 Y=A*X*A'
    vector<vector<double>> temp(ROW, vector<double>(COL, 0));
    for (int32_t i = 0; i < ROW; i++) {
        for (int32_t j = 0; j < COL; j++) {
            double sum = 0;
            for (int32_t k = 0; k < COL; k++) {
                sum += DCTAndIDCTArray[i][k] * originMatrix[k][j];
            }
            temp[i][j] = sum;
        }
    }
    for (int32_t i = 0; i < ROW; i++) {
        for (int32_t j = 0; j < COL; j++) {
            double sum = 0;
            for (int32_t k = 0; k < COL; k++) {
                sum += temp[i][k] * DCTAndIDCTArray[j][k];
            }
            originMatrix[i][j] = sum;
        }
    }
}

void JPEGData::IDCT(double** originMatrix)
{
    // 原理X=A'*Y*A
    vector<vector<double>> temp(ROW, vector<double>(COL, 0));
    for (int32_t i = 0; i < ROW; i++) {
        for (int32_t j = 0; j < COL; j++) {
            double sum = 0;
            for (int32_t k = 0; k < COL; k++) {
                sum += DCTAndIDCTArray[k][i] * originMatrix[k][j];
            }
            temp[i][j] = sum;
        }
    }
    for (int32_t i = 0; i < ROW; i++) {
        for (int32_t j = 0; j < COL; j++) {
            double sum = 0;
            for (int32_t k = 0; k < COL; k++) {
                sum += temp[i][k] * DCTAndIDCTArray[k][j];
            }
            originMatrix[i][j] = sum;
        }
    }
}

bool JPEGData::encodeProcess(fstream& file, uint16_t& pLen, uint16_t pType) {
    bool flag = true;
    switch (pType) {
        case SOF0:
        case SOF1:
        case SOF2: {
            flag = SOFProcess(file, pLen);
            break;
        }
        case DHT: {
            flag = DHTProcess(file, pLen);
            break;
        }
        // case SOI:
        // case EOI:
        case SOS: {
            flag = SOSProcess(file, pLen);
            if (!flag) // 正式读取数据
                break;
            flag = readData(file);
            break;
        }
        case DQT: {
            flag = DQTProcess(file, pLen);
            break;
        }
        case DRI: {
            DRIProcess(file, 2);
            break;
        }
        case APP0:
        case APP1:
        case APP2:
        case COM:
        default:
            pLen -= 2;
            while (pLen--) {
                file.get();
            }
            break;
    }
    return flag;
}

bool JPEGData::SOFProcess(fstream& file, uint16_t pLen) {
    return readSOF(file, pLen - 2);
}

bool JPEGData::DHTProcess(fstream& file, uint16_t pLen) {
    JPEGHuffmanCode huf;
    int32_t info = file.get();
    int32_t tableId = info & 0x0f;
    bool result = huf.Init(file, pLen - 3);
    if ((info >> 4) & 1)
        ac_huffman[tableId] = huf;
    else
        dc_huffman[tableId] = huf;
    return result;
}

bool JPEGData::SOSProcess(fstream& file, uint16_t pLen) {
    bool result = scan.Init(file, pLen - 2);
    int32_t count = 3;
    while (count--)
        file.get();
    if (!result) // 正式读取数据
        return false;
    return true;
}

bool JPEGData::DQTProcess(fstream& file, uint16_t pLen) {
    JPEGQuality q;
    bool result = q.Init(file, pLen - 2);
    quality.push_back(q);
    return result;
}

void JPEGData::DRIProcess(fstream& file, uint16_t pLen) {
    resetInterval = ReadByte(file, pLen);
}

string JPEGData::FlagCheck(fstream& file, int32_t byteInfo)
{
    if (byteInfo == 0xff) {
        uint8_t info = file.get();
        string res = bitset<8>(0xFF).to_string();
        if (info == 0xD9) {
            EOI = true;
            return "false";
        } else if (info == 0x00)
            return res;
        return res + bitset<8>(info).to_string();
    }
    return bitset<8>(byteInfo).to_string();
}

uint16_t JPEGData::ReadByte(fstream& file, int32_t len)
{
    uint16_t res = file.get();
    if (len != 1) {
        res = (res << 8) + (uint8_t)file.get();
    }
    return res;
}

uint16_t JPEGData::findHuffmanCodeByBit(
    fstream& file, int32_t& length, int32_t& pos, string& deque, int32_t curValue, int32_t& curValLen)
{
    if (pos == length && length >= HUFFMAN_DECODE_DEQUE_CACHE) { // 达到最大缓存
        deque = deque.substr(pos);
        int32_t info = file.get();
        string res = FlagCheck(file, info);
        string str = bitset<8>(info).to_string();
        if (res == "false")
            res = bitset<8>(file.get()).to_string();
        deque.append(res);
        length = deque.length();
        pos = 0;
    } else if (length == 0 || pos >= length) {
        if (length == 0) {
            deque = "";
            pos = 0;
        }
        int32_t info = file.get();
        string res = FlagCheck(file, info);
        string str = bitset<8>(info).to_string();
        if (res == "false")
            res = bitset<8>(file.get()).to_string();
        deque.append(res);
        length += 8;
    }
    curValue = (curValue << 1) + (uint8_t)(deque.at(pos++) - '0');
    curValLen++;
    return curValue;
}

void JPEGData::createDCEnHuffman()
{
    JPEGHuffmanEncode table1, table2;
    int32_t curCode = 0, curPos = 0;
    for (int32_t i = 0; i < 16; i++) {
        int32_t count = bits_dc_luminance[i];
        curCode <<= 1;
        while (count--) {
            uint16_t code = curCode++;
            uint8_t bit = i + 1, weight = val_dc_luminance[curPos++];
            pair<uint16_t, uint8_t> t1(code, bit);
            pair<uint8_t, pair<uint16_t, uint8_t>> t2(weight, t1);
            table1.table.insert(t2);
        }
    }
    curCode = curPos = 0;
    for (int32_t i = 0; i < 16; i++) {
        int32_t count = bits_dc_chrominance[i];
        curCode <<= 1;
        while (count--) {
            uint16_t code = curCode++;
            uint8_t bit = i + 1, weight = val_dc_chrominance[curPos++];
            pair<uint16_t, uint8_t> t1(code, bit);
            pair<uint8_t, pair<uint16_t, uint8_t>> t2(weight, t1);
            table2.table.insert(t2);
        }
    }
    en_dc_huffman.push_back(table1);
    en_dc_huffman.push_back(table2);
}
void JPEGData::createACEnHuffman()
{
    JPEGHuffmanEncode table1, table2;
    int32_t curCode = 0, curPos = 0;
    for (int32_t i = 0; i < 16; i++) {
        int32_t count = bits_ac_luminance[i];
        curCode <<= 1;
        while (count--) {
            uint16_t code = curCode++;
            uint8_t bit = i + 1, weight = val_ac_luminance[curPos++];
            pair<uint16_t, uint8_t> t1(code, bit);
            pair<uint8_t, pair<uint16_t, uint8_t>> t2(weight, t1);
            table1.table.insert(t2);
        }
    }
    curCode = curPos = 0;
    for (int32_t i = 0; i < 16; i++) {
        int32_t count = bits_ac_chrominance[i];
        curCode <<= 1;
        while (count--) {
            uint16_t code = curCode++;
            uint8_t bit = i + 1, weight = val_ac_chrominance[curPos++];
            pair<uint16_t, uint8_t> t1(code, bit);
            pair<uint8_t, pair<uint16_t, uint8_t>> t2(weight, t1);
            table2.table.insert(t2);
        }
    }
    en_ac_huffman.push_back(table1);
    en_ac_huffman.push_back(table2);
}

void JPEGData::initQualityTable(int32_t quality_scale)
{
    if (quality_scale <= 0)
        quality_scale = 1;
    else if (quality_scale >= 100)
        quality_scale = 99;
    for (int32_t i = 0; i < ROW * COL; i++) {
        uint8_t temp = ((uint8_t)(YQualityTable[i] * quality_scale + 50) / 100);
        if (temp <= 0)
            temp = 1;
        if (temp > 0xFF)
            temp = 0xFF;
        YQualityTable[i] = temp;

        temp = ((uint8_t)(CQualityTable[i] * quality_scale + 50) / 100);
        if (temp <= 0)
            temp = 1;
        if (temp > 0xFF)
            temp = 0xFF;
        CQualityTable[i] = temp;
    }
}

NAME_SPACE_END()