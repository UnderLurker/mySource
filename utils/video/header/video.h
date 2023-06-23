#ifndef _VIDEO_
#define _VIDEO_

#include "Util.h"
#include <list>
#include <memory>
#include <stdint.h>
NAME_SPACE_START(myUtil)

#define TYPE_LEN 4

class BoxHeader{
public:
    uint32_t size{0};//box大小包含header
    uint32_t type{0};//box类型 使用的是16进制转ASCII字符
    uint64_t largeSize{0};//size=0时使用(box大小超过size范围)
    uint64_t extended_type_high{0};//拓展类型高8字节
    uint64_t extended_type_low{0};//拓展类型低8字节
    BoxHeader(){}
    BoxHeader(const uint8_t _type[4]){
        type=0;
        for(int i=0;i<TYPE_LEN;i++){
            type<<=8;
            type|=(uint8_t)_type[i];
        }
    }
};

class FullBoxHeader : public BoxHeader{
public:
    uint8_t version{0};
    uint32_t flags{0};//标志位只占24位
    FullBoxHeader():BoxHeader(){}
    FullBoxHeader(const uint8_t _type[4]):BoxHeader(_type){}
};

struct BoxBody{

};

//only one
class FileTypeBox : public BoxHeader{
    uint8_t type_name[4]{'f','t','p','y'};
public:
    uint32_t major_brand{0};
    uint32_t minor_version{0};
    shared_ptr<uint32_t[]> compatible_brands;
    FileTypeBox():BoxHeader(type_name){}
};

// only one or zero
class ProgressiveDownloadInfoBox : public FullBoxHeader{
    uint8_t type_name[4]{'p','d','i','n'};
public:
    ProgressiveDownloadInfoBox():FullBoxHeader(type_name){}
//     aligned(8) class ProgressiveDownloadInfoBox 
//  extends FullBox(‘pdin’, version = 0, 0) { 
//  for (i=0; ; i++) { // to end of box 
//  unsigned int(32) rate; 
//  unsigned int(32) initial_delay; 
//  } 
// }
};

//only one
class MovieBox : public BoxHeader{
    uint8_t type_name[4]{'m','o','o','v'};
private:
    MovieBox():BoxHeader(type_name){};
};


//only one
class MovieHeaderBox : public FullBoxHeader{
    uint8_t type_name[4]{'m','v','h','d'};
private:
    bool isVersion1;//true 以下3个64位的属性为64位 false 32位
    uint64_t creation_time;
    uint64_t modification_time;
    uint32_t timescale;
    uint64_t duration;
    uint32_t rate{0x00010000};//1.0
    uint16_t volume{0x0100};//1.0 	indicates the preferred playback volume. 1.0 (0x0100) is full volume.
    uint16_t reserved{0};//没写完
    MovieHeaderBox():FullBoxHeader(type_name){};
};

//mp4文件的组成部分
class Box{
public:
    BoxHeader header;
    BoxBody body;
    shared_ptr<Box> next;//并列的box指针
    list<shared_ptr<Box>> child;//包含box的box指针
};

class Video{
    list<shared_ptr<Box>> boxes;
};


NAME_SPACE_END()

#endif
