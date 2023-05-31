#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#ifndef _QR_ENCODE_
#define _QR_ENCODE_

#include "Util.h"
#include "Matrix.h"
#include "Image.h"

NAME_SPACE_START(myUtil)
using namespace std;

// Step 1: Choose the Error Correction Level
// Step 2: Determine the Smallest Version for the Data
// Step 3: Add the Mode Indicator
// Step 4: Add the Character Count Indicator
//  The character count indicator must be placed after the mode indicator.
// Step 3: Encode Using the Selected Mode
// Step 4: Break Up into 8-bit Codewords and Add Pad Bytes if Necessary

#define VERSION_COUNT 40
#define MODE_INDICATOR_BIT_LENGTH 4
//对应第几行
enum ErrorCorrectionLevel{
    L = 0,  // Recovers 7% of data
    M = 1,  // Recovers 15% of data
    Q = 2,  // Recovers 25% of data
    H = 3   // Recovers 30% of data
};
//对应第几列
enum DataType{
    NumMode=0,
    AlpNumMode,
    ByteMode,
    KanjiMode
};
// Mode Indicator	Character Count Indicator	Encoded Data

//QRCode Mode Indicator 对应DataType, 此编码长度为MODE_INDICATOR_BIT_LENGTH
//The encoded data must start with the appropriate mode indicator 
//that specifies the mode being used for the bits that come after it.
const static int ModeIndicator[]={
    //NumMode AlpNumMode ByteMode KanjiMode ECIMode
    1,2,4,8,7
};

// 将编码数据的长度编译为对应长度的二进制位，共有三种长度，每种长度中对应4种DataType
// The character count indicator is a string of bits that 
// represents the number of characters that are being encoded.
const static int CharCountIndicator[3][4]{
    {10,9,8,8},     // version 1~9
    {12,11,16,10},  // version 10~26
    {14,13,16,12},  // version 27~40
};


//Character Capacities by Version, Mode, and Error Correction
const static int CharCapTable[40][4][4]={
    //L M   Q   H
    //Numeric mode,Alphanumeric mode,Byte mode,Kanji mode
    {{41,25,17,10},{34,20,14,8},{27,16,11,7},{17,10,7,4}},
    {{77,47,32,20},{63,38,26,16},{48,29,20,12},{34,20,14,8}},
    {{127,77,53,32},{101,61,42,26},{77,47,32,20},{58,35,24,15}},
    {{187,114,78,48},{149,90,62,38},{111,67,46,28},{82,50,34,21}},
    {{255,154,106,65},{202,122,84,52},{144,87,60,37},{106,64,44,27}},
    {{322,195,134,82},{255,154,106,65},{178,108,74,45},{139,84,58,36}},
    {{370,224,154,95},{293,178,122,75},{207,125,86,53},{154,93,64,39}},
    {{461,279,192,118},{365,221,152,93},{259,157,108,66},{202,122,84,52}},
    {{552,335,230,141},{432,262,180,111},{312,189,130,80},{235,143,98,60}},
    {{652,395,271,167},{513,311,213,131},{364,221,151,93},{288,174,119,74}},
    {{772,468,321,198},{604,366,251,155},{427,259,177,109},{331,200,137,85}},
    {{883,535,367,226},{691,419,287,177},{489,296,203,125},{374,227,155,96}},
    {{1022,619,425,262},{796,483,331,204},{580,352,241,149},{427,259,177,109}},
    {{1101,667,458,282},{871,528,362,223},{621,376,258,159},{468,283,194,120}},
    {{1250,758,520,320},{991,600,412,254},{703,426,292,180},{530,321,220,136}},
    {{1408,854,586,361},{1082,656,450,277},{775,470,322,198},{602,365,250,154}},
    {{1548,938,644,397},{1212,734,504,310},{876,531,364,224},{674,408,280,173}},
    {{1725,1046,718,442},{1346,816,560,345},{948,574,394,243},{746,452,310,191}},
    {{1903,1153,792,488},{1500,909,624,384},{1063,644,442,272},{813,493,338,208}},
    {{2061,1249,858,528},{1600,970,666,410},{1159,702,482,297},{919,557,382,235}},
    {{2232,1352,929,572},{1708,1035,711,438},{1224,742,509,314},{969,587,403,248}},
    {{2409,1460,1003,618},{1872,1134,779,480},{1358,823,565,348},{1056,640,439,270}},
    {{2620,1588,1091,672},{2059,1248,857,528},{1468,890,611,376},{1108,672,461,284}},
    {{2812,1704,1171,721},{2188,1326,911,561},{1588,963,661,407},{1228,744,511,315}},
    {{3057,1853,1273,784},{2395,1451,997,614},{1718,1041,715,440},{1286,779,535,330}},
    {{3283,1990,1367,842},{2544,1542,1059,652},{1804,1094,751,462},{1425,864,593,365}},
    {{3517,2132,1465,902},{2701,1637,1125,692},{1933,1172,805,496},{1501,910,625,385}},
    {{3669,2223,1528,940},{2857,1732,1190,732},{2085,1263,868,534},{1581,958,658,405}},
    {{3909,2369,1628,1002},{3035,1839,1264,778},{2181,1322,908,559},{1677,1016,698,430}},
    {{4158,2520,1732,1066},{3289,1994,1370,843},{2358,1429,982,604},{1782,1080,742,457}},
    {{4417,2677,1840,1132},{3486,2113,1452,894},{2473,1499,1030,634},{1897,1150,790,486}},
    {{4686,2840,1952,1201},{3693,2238,1538,947},{2670,1618,1112,684},{2022,1226,842,518}},
    {{4965,3009,2068,1273},{3909,2369,1628,1002},{2805,1700,1168,719},{2157,1307,898,553}},
    {{5253,3183,2188,1347},{4134,2506,1722,1060},{2949,1787,1228,756},{2301,1394,958,590}},
    {{5529,3351,2303,1417},{4343,2632,1809,1113},{3081,1867,1283,790},{2361,1431,983,605}},
    {{5836,3537,2431,1496},{4588,2780,1911,1176},{3244,1966,1351,832},{2524,1530,1051,647}},
    {{6153,3729,2563,1577},{4775,2894,1989,1224},{3417,2071,1423,876},{2625,1591,1093,673}},
    {{6479,3927,2699,1661},{5039,3054,2099,1292},{3599,2181,1499,923},{2735,1658,1139,701}},
    {{6743,4087,2809,1729},{5313,3220,2213,1362},{3791,2298,1579,972},{2927,1774,1219,750}},
    {{7089,4296,2953,1817},{5596,3391,2331,1435},{3993,2420,1663,1024},{3057,1852,1273,784}},
};

static map<uint8_t,uint8_t> AlpValMappingTable={
    {0,0},{1,1},{2,2},{3,3},{4,4},{5,5},{6,6},{7,7},{8,8},{9,9},
    {'A',10},{'B',11},{'C',12},{'D',13},{'E',14},{'F',15},{'G',16},{'H',17},{'I',18},
    {'J',19},{'K',20},{'L',21},{'M',22},{'N',23},{'O',24},{'P',25},{'Q',26},{'R',27},
    {'S',28},{'T',29},{'U',30},{'V',31},{'W',32},{'X',33},{'Y',34},{'Z',35},{' ',36},
    {'$',37},{'%',38},{'*',39},{'+',40},{'-',41},{'.',42},{'/',43},{':',44}
};

class QREncode{
public:
    QREncode(){}
    QREncode(const string& imageFilePath):_imageFilePath(imageFilePath){}
    ~QREncode(){}
    Matrix<RGB> encoding(const string& encodeData);
    string decoding(const string& outputPath);

    int getSideLen() { return 21 + 4 * (version - 1); }
protected:
    bool init();
    string NumModeEncoding();
    string AlpNumModeEncoding();
    string ByteModeEncoding();
    string KanjiModeEncoding();
private:
    string encodeData;
    string _imageFilePath;
    ErrorCorrectionLevel level{L};
    DataType type{KanjiMode};
    int version{0}; //matrix sideLen: 21+4*(version-1)
    int charCount{0};
    Matrix<RGB> *imgData{nullptr};
};

NAME_SPACE_END()

#endif //!_QR_ENCODE_