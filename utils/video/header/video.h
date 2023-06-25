#ifndef _VIDEO_
#define _VIDEO_

#include "Util.h"
#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <stdint.h>
NAME_SPACE_START(myUtil)

#define TYPE_LEN 4

//32位
struct Bit32{
    using bit_type = uint32_t;
};

//64位
struct Bit64{
    using bit_type = uint64_t;
};

class BoxHeader{
private:
    uint32_t container={0};
public:
    uint32_t size{0};//box大小包含header
    uint32_t type{0};//box类型 使用的是16进制转ASCII字符
    uint64_t largeSize{0};//size=0时使用(box大小超过size范围)
    uint64_t extended_type_high{0};//拓展类型高8字节
    uint64_t extended_type_low{0};//拓展类型低8字节
    BoxHeader(){}
    BoxHeader(const string _type){
        type=0;
        for(int i=0;i<TYPE_LEN;i++){
            type<<=8;
            type|=(uint8_t)_type[i];
        }
    }
    BoxHeader(const uint32_t _type){
        type=_type;
    }
    BoxHeader(const string _type,const string _container){
        type=0;
        container=0;
        for(int i=0;i<TYPE_LEN;i++){
            type<<=8;
            type|=(uint8_t)_type[i];
        }
        for(int i=0;i<TYPE_LEN;i++){
            container<<=8;
            container|=(uint8_t)_container[i];
        }
    }
};
template<typename T>
class FullBoxHeader : public BoxHeader{
protected:
    //T类型要依靠version来确定 version=1 64位 否则为 32位
    using bit_type = typename T::bit_type;
public:
    uint8_t version{0};
    uint32_t flags{0};//标志位只占24位
    FullBoxHeader():BoxHeader(){}
    FullBoxHeader(const string _type,const uint32_t _flags)
            :BoxHeader(_type),flags(_flags){}
    FullBoxHeader(const string _type,const uint8_t _version,const uint32_t _flags)
            :BoxHeader(_type),version(_version),flags(_flags){}
    FullBoxHeader(const string _type,const string _container)
            :BoxHeader(_type,_container){}
    FullBoxHeader(const string _type,const string _container,const uint32_t _flags)
            :BoxHeader(_type,_container),flags(_flags){}
    FullBoxHeader(const string _type,const string _container,const uint8_t _version,const uint32_t _flags)
            :BoxHeader(_type,_container),version(_version),flags(_flags){}
};

struct BoxBody{

};

//only one
class FileTypeBox : public BoxHeader{
public:
    uint32_t major_brand{0};
    uint32_t minor_version{0};
    shared_ptr<uint32_t[]> compatible_brands;
    FileTypeBox():BoxHeader("ftyp","File"){}
};

// only one or zero
template<typename T = Bit32>
class ProgressiveDownloadInfoBox : public FullBoxHeader<T>{
public:
    ProgressiveDownloadInfoBox():FullBoxHeader<T>("pdin","File",0,0){}
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
private:
    MovieBox():BoxHeader("moov","File"){};
};


//only one
template<typename T>
class MovieHeaderBox : public FullBoxHeader<T>{
    using bit_type = typename FullBoxHeader<T>::bit_type;
private:
    bit_type creation_time;
    bit_type modification_time;
    uint32_t timescale;
    bit_type duration;
    uint32_t rate{0x00010000};//1.0
    uint16_t volume{0x0100};//1.0 	indicates the preferred playback volume. 1.0 (0x0100) is full volume.
    const uint16_t reserved{0};//没写完
    const uint32_t reservedList[2]{0};
    uint32_t matrix[9]{0x00010000,0,0,0,0x00010000,0,0,0,0x40000000};
    uint32_t pre_defined[6]{0};
    uint32_t next_track_ID{0};//非0值
    MovieHeaderBox():FullBoxHeader<T>("mvhd","moov",(uint32_t)0){};
    MovieHeaderBox(const uint8_t _version):FullBoxHeader<T>("mvhd","moov",_version,0){};
};

//quantity one or more
class TrackBox : public BoxHeader{
public:
    TrackBox():BoxHeader("trak","moov"){}
};
// only one
template<typename T>
class TrackHeaderBox : public FullBoxHeader<T>{
    using bit_type = typename FullBoxHeader<T>::bit_type;
public:
    bit_type creation_time;
    bit_type modification_time;
    uint32_t track_ID;
    const uint32_t reserved{0};
    bit_type duration;
    const uint32_t reservedList[2]{0};
    uint16_t layer{0};
    uint16_t alternate_group{0};
    // if track_is_audio 0x0100 else 0
    uint16_t volume{0};
    const uint16_t reservedOther{0};
    uint32_t matrix[9]{0x00010000,0,0,0,0x00010000,0,0,0,0x40000000};
    uint32_t width;
    uint32_t height;
    TrackHeaderBox():FullBoxHeader<T>("tkhd","trak"){}
    TrackHeaderBox(const uint8_t _version,const uint32_t _flags)
        :FullBoxHeader<T>("tkhd","trak",_version,_flags){}
};

class TrackReferenceBox : public BoxHeader{
public:
    TrackReferenceBox():BoxHeader("tref","trak"){}
};

class TrackReferenceTypeBox : public BoxHeader{
public:
    //此值不能为0
    shared_ptr<uint32_t> track_IDs{nullptr};
    //reference_type 此值为如下几个值或者是注册过的或者是一个derived specification or registration
    //hint:the referenced tracks contain the original media for this hint track.
    //cdsc:this track describes the referenced track
    //font:the track uses fonts carried/defined in the referenced track.
    //hind:this track depends on the referenced hint track
    //vdep:this track contains auxiliary depth video information for the referenced video track
    //vplx:this track contain auxiliary parallax video information for the referenced video track
    //subt:this track contains subtitle, timed text or overlay graphical information for the referenced track or any track
    //in the alternate group to which the track belongs,if any
    TrackReferenceTypeBox(const string reference_type)
        :BoxHeader(reference_type){}
};

class TrackGroupBox : public BoxHeader{
public:
    TrackGroupBox():BoxHeader("trgr"){}
};

template<typename T = Bit32>
class TrackGroupTypeBox : public FullBoxHeader<T>{
public:
    uint32_t track_group_id;
    TrackGroupTypeBox(const string track_group_type)
        :FullBoxHeader<T>("trgr",track_group_type,0,0){};
};

class MediaBox : public BoxHeader{
public:
    MediaBox():BoxHeader("mdia","trak"){}
};

template<typename T>
class MediaHeaderBox : public FullBoxHeader<T>{
    using bit_type = typename FullBoxHeader<T>::bit_type;
public:
    bit_type creation_time;
    bit_type modification_time;
    uint32_t timescale;
    bit_type duration;
    bool pad{false};
    uint16_t language;//ISO-639-2/T language code 每个值为5位
    uint16_t pre_defined{0};
    MediaHeaderBox(const uint8_t _version)
        :FullBoxHeader<T>("mdhd","mdia",_version,0){};
};

template<typename T = Bit32>
class HandlerReferenceBox : FullBoxHeader<T>{
public:
    uint32_t pre_defined{0};
    uint32_t handler_type;
    uint32_t reserved[3]={0};
    string name;
    HandlerReferenceBox(const string _container)
        :FullBoxHeader<T>("hdlr",_container,0,0){}
};

class MediaInformationBox : public BoxHeader{
public:
    MediaInformationBox():BoxHeader("minf","mdia"){}
};

template<typename T = Bit32>
class NullMediaHeaderBox : public FullBoxHeader<T>{
public:
    NullMediaHeaderBox(const uint32_t _flag):FullBoxHeader<T>("nmhd","minf",0,_flag){}
};

template<typename T = Bit32>
class ExtendLanguageTag : public FullBoxHeader<T>{
public:
    string extended_language;//such as "en-US","zh-CN"
    ExtendLanguageTag():FullBoxHeader<T>("elng","mdia",0,0){}
};

class SampleTableBox : public BoxHeader{
public:
    SampleTableBox():BoxHeader("stbl","minf"){}
};

class SampleEntry : public BoxHeader{
public:
    uint8_t reserved[6]{0};
    uint16_t data_reference_index;
    SampleEntry(const string format):BoxHeader(format){}
};

class BitRateBox : public BoxHeader{
public:
    uint32_t bufferSizeDB; 
    uint32_t maxBitrate; 
    uint32_t avgBitrate;
    BitRateBox():BoxHeader("btrt"){}
};

template<typename T>
class SampleDescriptionBox : public FullBoxHeader<T>{
public:
    uint32_t entry_count;
    SampleDescriptionBox(const uint8_t _version)
        :FullBoxHeader<T>("stsd","stbl",_version,0){}
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
