#ifndef _VIDEO_
#define _VIDEO_

#include "Util.h"
#include <list>
#include <memory>
#include <stdint.h>
NAME_SPACE_START(myUtil)

struct BoxHeader{
    uint32_t size{0};//box大小包含header
    uint32_t type{0};//box类型
    uint64_t largeSize{0};//size=0时使用(box大小超过size范围)
    uint64_t extended_type_high{0};//拓展类型高8字节
    uint64_t extended_type_low{0};//拓展类型低8字节
    uint8_t version{0};
    uint32_t flags{0};//标志位只占24位
};

struct BoxBody{

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
