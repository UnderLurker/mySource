#pragma once

#ifndef _MUSIC_
#define _MUSIC_

#include "Util.h"

NAME_SPACE_START(myUtil)

struct MP3FrameHeader
{
    uint8_t sync1:8;            //同步信息 1
    uint8_t error_protection:1; //CRC 校验
    uint8_t layer:2;            //层
    uint8_t version:2;          //版本
    uint8_t sync2:3;            //同步信息 2
    uint8_t extension:1;        //版权
    uint8_t padding:1;          //填充空白字
    uint8_t sample_rate_index:2;//采样率索引
    uint8_t bit_rate_index:4;   //位率索引
    uint8_t emphasis:2;         //强调方式
    uint8_t original:1;         //原始媒体
    uint8_t copyright:1;        //版权标志
    uint8_t mode_extension:2;   //扩展模式,仅用于联合立体声
    uint8_t channel_mode:2;     //声道模式
};

//base
class Music{
public:
    Music(const string& _path):filePath(_path){}
    virtual bool analysis(){return false;}
    string filePath;
};

class MP3 : protected Music{
    MP3FrameHeader head;
public:
    MP3(const string& _path):Music(_path){}
    bool analysis();
};

NAME_SPACE_END()

#endif //!_MUSIC_