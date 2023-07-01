#include <condition_variable>
#include <mutex>
#define _CRT_SECURE_NO_WARNINGS
#include "Util.h"
#include <algorithm>
#include <atomic>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <exception>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <iterator>
#include <locale>
#include <cmath>
#include <map>
#include <ostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <streambuf>
#include <string>
#include <thread>
#include <time.h>

NAME_SPACE_START(myUtil)
IniValue::IniValue()
{

}

IniValue::IniValue(const bool val){
    m_value=val?"true":"false";
}

IniValue::IniValue(const int val){
    m_value=std::to_string(val);
}

IniValue::IniValue(const char* val){
    m_value=val;
}

IniValue::IniValue(const std::string val){
    m_value=val;
}

IniValue& IniValue::operator=(const bool& val){
    IniValue temp(val);
    m_value=temp.m_value;
    return *this;
}

IniValue& IniValue::operator=(const int& val){
    IniValue temp(val);
    m_value=temp.m_value;
    return *this;
}
IniValue& IniValue::operator=(const char* val){
    m_value=val;
    return *this;
}
IniValue& IniValue::operator=(const std::string& val){
    m_value=val;
    return *this;
}

IniValue::operator bool() const{
    return m_value=="true"?true:false;
}

IniValue::operator int() const{
    std::stringstream ss(m_value);
    int res=0;
    ss>>res;
    return res;
}

IniValue::operator std::string() const{
    return m_value;
}

IniHelper::IniHelper(){
    loadIniFile();
}
IniHelper::IniHelper(const std::string& filePath){
    m_filePath=filePath;
    loadIniFile();
}

void IniHelper::saveIniConfig(){
    std::fstream file;
    file.open(m_filePath,std::ios_base::out);
    if(file.fail()) return;
    for(auto it=m_map.begin();it!=m_map.end();it++){
        std::string sectionName="["+it->first+"]\n";
        file<<sectionName;
        auto keySection = it->second;
        for(auto key=keySection.begin();key!=keySection.end();key++){
            std::string keyValue=key->first;
            keyValue.append("=").append(key->second.m_value).append("\n");
            file<<keyValue;
        }
    }
    file.close();
}

void IniHelper::LogIniConfig() const{
    for(auto it=m_map.begin();it!=m_map.end();it++){
        std::string sectionName="["+it->first+"]\n";
        std::cout<<sectionName;
        auto keySection = it->second;
        for(auto key=keySection.begin();key!=keySection.end();key++){
            std::string keyValue=key->first;
            keyValue.append("=").append(key->second.m_value).append("\n");
            std::cout<<keyValue;
        }
    }
}

void IniHelper::setIniConfig(const std::string& sectionName,const std::string& keyName,const std::string& value){
    if(m_map.find(sectionName)==m_map.end()){
        std::map<std::string, IniValue> temp;
        temp[keyName]=value;
        m_map[sectionName]=temp;
    }
    else{
        m_map[sectionName][keyName]=value;
    }
}

IniValue IniHelper::getIniConfig(const std::string& sectionName,const std::string& keyName,const std::string& defValue) const{
    if(m_map.find(sectionName)==m_map.end()) return defValue;
    std::map<std::string, IniValue> mapping=m_map.at(sectionName);
    if(mapping.find(keyName)==mapping.end()) return defValue;
    return mapping[keyName];
}

bool IniHelper::removeIniConfig(const std::string& sectionName,const std::string& keyName){
    try {
        if(m_map.find(sectionName)==m_map.end()) return true;
        auto pos = m_map.at(sectionName).find(keyName);
        if(pos==m_map.at(sectionName).end()) return true;
        m_map.at(sectionName).erase(pos);
        return true;
    } catch (std::exception ex) {
        //std::cout<<ex.what()<<std::endl;
        return false;
    }
}

void IniHelper::loadIniFile(){
    std::fstream file;
    file.open(m_filePath,std::ios_base::in);
    if(file.fail()) return;
    std::string sectionName="",t="";
    while(std::getline(file,t)){
        trim(t);        //去除前后空格
        if(t=="\n"||t=="") continue;
        else if(t[0]=='['){
            sectionName = t.substr(1,t.size()-2);
            std::map<std::string, IniValue> p;
            m_map[sectionName]=p;
        }
        else{
            if(sectionName=="") continue;
            int left=0,right=0,equalPos=0;
            equalPos=t.find("=");
            if(equalPos==std::string::npos) continue;
            std::string keyName="",keyValue="";
            keyName=t.substr(0,equalPos);
            keyValue=t.substr(equalPos+1,t.size()-1-equalPos);
            trim(keyName);
            trim(keyValue);
            m_map[sectionName][keyName]=keyValue;
        }
    }
    file.close();
}

void IniHelper::trim(std::string& str){
    REMOVE_SPACE(str);
    std::reverse(str.begin(), str.end());
    REMOVE_SPACE(str);
    std::reverse(str.begin(), str.end());
}

std::string getFile(const std::string& filePath)
{
    std::string content = "";
    std::fstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.fail()) {
        while (!file.eof()) {
            char ch;
            file.get(ch);
            content.push_back(ch);
        }
    }
    file.close();
    return content;
}

Timer::Timer(){
    startTime=clock();
}

Timer::Timer(void* lpFunc){
    startTime=clock();
    this->lpFunc=lpFunc;
}

std::string Timer::getRunTime(const std::string &format){
    curTime=clock();
    clock_t temp=curTime-startTime;
    if(format=="millisecond") return std::to_string(temp);
    else if(format=="second") return std::to_string(temp/CLOCKS_PER_SEC);
    return std::to_string(temp);
}

void Timer::setInterval(void *lpFunc,long interval){
    runFlag=true;
    this->lpFunc=lpFunc;
    this->inter=interval;
}

template<typename classType, typename ...Args>
void Timer::start(Args... args){
    typedef std::function<void(classType,Args...)> classFunc;
    classFunc* func=(classFunc*)lpFunc;
    while(runFlag){
        // Sleep(inter);
        // std::thread th([](Args... args){
        //     (*func)(args...);
        // },args...);
        // th.join();
    }
}

void Timer::end(){
    runFlag=false;
}

vector<string> Split(const string& source, char ch){
    vector<string> res;
    size_t pre=0,pos=0;
    while((pos=source.find(ch,pre))!=string::npos){
        string str(source.begin()+pre,source.begin()+pos);
        res.push_back(str);
        pre=pos+1;
    }
    if(pre!=0) res.push_back(source.substr(pre));
    return res;
}
vector<string> Split(const char* source,size_t len,char ch){
    string str(source);
    return Split(str,ch);
}
vector<wstring> Split(const wstring& source, wchar_t ch){
    vector<wstring> res;
    size_t pre=0,pos=0;
    while((pos=source.find(ch,pre))!=wstring::npos){
        wstring str(source.begin()+pre,source.begin()+pos);
        res.push_back(str);
        pre=pos+1;
    }
    if(pre!=0) res.push_back(source.substr(pre));
    return res;
}
vector<wstring> Split(const wchar_t* source,size_t len,wchar_t ch){
    wstring str(source);
    return Split(str,ch);
}

#define F(x, y, z)   ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)   ((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)   ((x) ^ (y) ^ (z))
#define I(x, y, z)   ((y) ^ ((x) | ~(z)))
#define STEP(f, a, b, c, d, x, t, s) \
    (a) += f((b), (c), (d)) + (x) + (t); \
    (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
    (a) += (b);

#if defined(__i386__) || defined(__x86_64__) || defined(__vax__)
    #define SET(n) \
        (*(uint32_t *)&ptr[(n) * 4])
    #define GET(n) \
        SET(n)
#else
    #define SET(n) \
        (ctx->block[(n)] = \
        (uint32_t)ptr[(n) * 4] | \
        ((uint32_t)ptr[(n) * 4 + 1] << 8) | \
        ((uint32_t)ptr[(n) * 4 + 2] << 16) | \
        ((uint32_t)ptr[(n) * 4 + 3] << 24))
    #define GET(n) \
        (ctx->block[(n)])
#endif

typedef struct {
    uint32_t lo, hi;
    uint32_t a, b, c, d;
    unsigned char buffer[64];
    uint32_t block[16];
} MD5_CTX;

static void MD5_Init(MD5_CTX *ctx);
static void MD5_Update(MD5_CTX *ctx, const void *data, unsigned long size);
static void MD5_Final(uint8_t *result, MD5_CTX *ctx);

static const void *body(MD5_CTX *ctx, const void *data, unsigned long size){
    const unsigned char *ptr;
    uint32_t a, b, c, d;
    uint32_t saved_a, saved_b, saved_c, saved_d;

    ptr = (const unsigned char*)data;

    a = ctx->a;
    b = ctx->b;
    c = ctx->c;
    d = ctx->d;

    do {
        saved_a = a;
        saved_b = b;
        saved_c = c;
        saved_d = d;

        STEP(F, a, b, c, d, SET(0), 0xd76aa478, 7)
        STEP(F, d, a, b, c, SET(1), 0xe8c7b756, 12)
        STEP(F, c, d, a, b, SET(2), 0x242070db, 17)
        STEP(F, b, c, d, a, SET(3), 0xc1bdceee, 22)
        STEP(F, a, b, c, d, SET(4), 0xf57c0faf, 7)
        STEP(F, d, a, b, c, SET(5), 0x4787c62a, 12)
        STEP(F, c, d, a, b, SET(6), 0xa8304613, 17)
        STEP(F, b, c, d, a, SET(7), 0xfd469501, 22)
        STEP(F, a, b, c, d, SET(8), 0x698098d8, 7)
        STEP(F, d, a, b, c, SET(9), 0x8b44f7af, 12)
        STEP(F, c, d, a, b, SET(10), 0xffff5bb1, 17)
        STEP(F, b, c, d, a, SET(11), 0x895cd7be, 22)
        STEP(F, a, b, c, d, SET(12), 0x6b901122, 7)
        STEP(F, d, a, b, c, SET(13), 0xfd987193, 12)
        STEP(F, c, d, a, b, SET(14), 0xa679438e, 17)
        STEP(F, b, c, d, a, SET(15), 0x49b40821, 22)
        STEP(G, a, b, c, d, GET(1), 0xf61e2562, 5)
        STEP(G, d, a, b, c, GET(6), 0xc040b340, 9)
        STEP(G, c, d, a, b, GET(11), 0x265e5a51, 14)
        STEP(G, b, c, d, a, GET(0), 0xe9b6c7aa, 20)
        STEP(G, a, b, c, d, GET(5), 0xd62f105d, 5)
        STEP(G, d, a, b, c, GET(10), 0x02441453, 9)
        STEP(G, c, d, a, b, GET(15), 0xd8a1e681, 14)
        STEP(G, b, c, d, a, GET(4), 0xe7d3fbc8, 20)
        STEP(G, a, b, c, d, GET(9), 0x21e1cde6, 5)
        STEP(G, d, a, b, c, GET(14), 0xc33707d6, 9)
        STEP(G, c, d, a, b, GET(3), 0xf4d50d87, 14)
        STEP(G, b, c, d, a, GET(8), 0x455a14ed, 20)
        STEP(G, a, b, c, d, GET(13), 0xa9e3e905, 5)
        STEP(G, d, a, b, c, GET(2), 0xfcefa3f8, 9)
        STEP(G, c, d, a, b, GET(7), 0x676f02d9, 14)
        STEP(G, b, c, d, a, GET(12), 0x8d2a4c8a, 20)
        STEP(H, a, b, c, d, GET(5), 0xfffa3942, 4)
        STEP(H, d, a, b, c, GET(8), 0x8771f681, 11)
        STEP(H, c, d, a, b, GET(11), 0x6d9d6122, 16)
        STEP(H, b, c, d, a, GET(14), 0xfde5380c, 23)
        STEP(H, a, b, c, d, GET(1), 0xa4beea44, 4)
        STEP(H, d, a, b, c, GET(4), 0x4bdecfa9, 11)
        STEP(H, c, d, a, b, GET(7), 0xf6bb4b60, 16)
        STEP(H, b, c, d, a, GET(10), 0xbebfbc70, 23)
        STEP(H, a, b, c, d, GET(13), 0x289b7ec6, 4)
        STEP(H, d, a, b, c, GET(0), 0xeaa127fa, 11)
        STEP(H, c, d, a, b, GET(3), 0xd4ef3085, 16)
        STEP(H, b, c, d, a, GET(6), 0x04881d05, 23)
        STEP(H, a, b, c, d, GET(9), 0xd9d4d039, 4)
        STEP(H, d, a, b, c, GET(12), 0xe6db99e5, 11)
        STEP(H, c, d, a, b, GET(15), 0x1fa27cf8, 16)
        STEP(H, b, c, d, a, GET(2), 0xc4ac5665, 23)
        STEP(I, a, b, c, d, GET(0), 0xf4292244, 6)
        STEP(I, d, a, b, c, GET(7), 0x432aff97, 10)
        STEP(I, c, d, a, b, GET(14), 0xab9423a7, 15)
        STEP(I, b, c, d, a, GET(5), 0xfc93a039, 21)
        STEP(I, a, b, c, d, GET(12), 0x655b59c3, 6)
        STEP(I, d, a, b, c, GET(3), 0x8f0ccc92, 10)
        STEP(I, c, d, a, b, GET(10), 0xffeff47d, 15)
        STEP(I, b, c, d, a, GET(1), 0x85845dd1, 21)
        STEP(I, a, b, c, d, GET(8), 0x6fa87e4f, 6)
        STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0, 10)
        STEP(I, c, d, a, b, GET(6), 0xa3014314, 15)
        STEP(I, b, c, d, a, GET(13), 0x4e0811a1, 21)
        STEP(I, a, b, c, d, GET(4), 0xf7537e82, 6)
        STEP(I, d, a, b, c, GET(11), 0xbd3af235, 10)
        STEP(I, c, d, a, b, GET(2), 0x2ad7d2bb, 15)
        STEP(I, b, c, d, a, GET(9), 0xeb86d391, 21)

        a += saved_a;
        b += saved_b;
        c += saved_c;
        d += saved_d;

        ptr += 64;
    } while (size -= 64);

    ctx->a = a;
    ctx->b = b;
    ctx->c = c;
    ctx->d = d;

    return ptr;
}

void MD5_Init(MD5_CTX *ctx){
    ctx->a = 0x67452301;
    ctx->b = 0xefcdab89;
    ctx->c = 0x98badcfe;
    ctx->d = 0x10325476;

    ctx->lo = 0;
    ctx->hi = 0;
}

void MD5_Update(MD5_CTX *ctx, const void *data, unsigned long size){
    uint32_t saved_lo;
    unsigned long used, free;

    saved_lo = ctx->lo;
    if ((ctx->lo = (saved_lo + size) & 0x1fffffff) < saved_lo)
        ctx->hi++;
    ctx->hi += size >> 29;
    used = saved_lo & 0x3f;

    if (used){
        free = 64 - used;
        if (size < free) {
            memcpy(&ctx->buffer[used], data, size);
            return;
        }

        memcpy(&ctx->buffer[used], data, free);
        data = (unsigned char *)data + free;
        size -= free;
        body(ctx, ctx->buffer, 64);
    }

    if (size >= 64) {
        data = body(ctx, data, size & ~(unsigned long)0x3f);
        size &= 0x3f;
    }

    memcpy(ctx->buffer, data, size);
}

void MD5_Final(uint8_t *result, MD5_CTX *ctx){
    unsigned long used, free;
    used = ctx->lo & 0x3f;
    ctx->buffer[used++] = 0x80;
    free = 64 - used;

    if (free < 8) {
        memset(&ctx->buffer[used], 0, free);
        body(ctx, ctx->buffer, 64);
        used = 0;
        free = 64;
    }

    memset(&ctx->buffer[used], 0, free - 8);

    ctx->lo <<= 3;
    ctx->buffer[56] = ctx->lo;
    ctx->buffer[57] = ctx->lo >> 8;
    ctx->buffer[58] = ctx->lo >> 16;
    ctx->buffer[59] = ctx->lo >> 24;
    ctx->buffer[60] = ctx->hi;
    ctx->buffer[61] = ctx->hi >> 8;
    ctx->buffer[62] = ctx->hi >> 16;
    ctx->buffer[63] = ctx->hi >> 24;
    body(ctx, ctx->buffer, 64);
    result[0] = ctx->a;
    result[1] = ctx->a >> 8;
    result[2] = ctx->a >> 16;
    result[3] = ctx->a >> 24;
    result[4] = ctx->b;
    result[5] = ctx->b >> 8;
    result[6] = ctx->b >> 16;
    result[7] = ctx->b >> 24;
    result[8] = ctx->c;
    result[9] = ctx->c >> 8;
    result[10] = ctx->c >> 16;
    result[11] = ctx->c >> 24;
    result[12] = ctx->d;
    result[13] = ctx->d >> 8;
    result[14] = ctx->d >> 16;
    result[15] = ctx->d >> 24;
    memset(ctx, 0, sizeof(*ctx));
}

/* Return Calculated raw result(always little-endian), the size is always 16 */
void md5bin(const void* dat, size_t len, uint8_t out[16]) {
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, dat, len);
    MD5_Final(out, &c);
}

static char hb2hex(unsigned char hb) {
    hb = hb & 0xF;
    return hb < 10 ? '0' + hb : hb - 10 + 'a';
}

string md5file(std::FILE* file){

	MD5_CTX c;
    MD5_Init(&c);

	char buff[BUFSIZ];
	uint8_t out[16];
	size_t len = 0;
	while( ( len = std::fread(buff ,sizeof(char), BUFSIZ, file) ) > 0) {
		MD5_Update(&c, buff, len);
	}
	MD5_Final(out, &c);

	string res;
	for(size_t i = 0; i < 16; ++ i) {
        res.push_back(hb2hex(out[i] >> 4));
        res.push_back(hb2hex(out[i]));
    }
	return res;
}

string MD5::encodeFile(const string& fileName){
	std::FILE* file = std::fopen(fileName.c_str(), "rb");
	string res = md5file(file);
	std::fclose(file);
	return res;
}


string md5(const void* dat, size_t len) {
    string res;
    uint8_t out[16];
    md5bin(dat, len, out);
    for(size_t i = 0; i < 16; ++ i) {
        res.push_back(hb2hex(out[i] >> 4));
        res.push_back(hb2hex(out[i]));
    }
    return res;
}

std::string MD5::encodeStr(const std::string& dat){
	return md5(dat.c_str(), dat.length());
}

// /* Generate shorter md5sum by something like base62 instead of base16 or base10. 0~61 are represented by 0-9a-zA-Z */
// string md5sum6(const void* dat, size_t len){
//     static const char* tbl = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
//     string res;
//     unsigned char out[16];
//     md5bin(dat, len, out);
//     for(size_t i = 0; i < 6; ++i) {
//         res.push_back(tbl[out[i] % 62]);
//     }
//     return res;
// }

// std::string md5sum6(std::string dat){
// 	return md5sum6(dat.c_str(), dat.length() );
// }

int ProgressBar::Logic::barLen = 100;
int ProgressBar::Logic::curLen = 0;
thread ProgressBar::run;
string ProgressBar::Logic::str = "";
bool ProgressBar::_pause = false;
char ProgressBar::Logic::ch = '=';
condition_variable ProgressBar::Logic::_cv;
mutex ProgressBar::barMutex;

NAME_SPACE_END()
