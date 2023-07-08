#ifndef _COMPRESS_
#define _COMPRESS_

#include "Util.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <list>
#include <memory>
#include <stdint.h>

NAME_SPACE_START(myUtil)

enum Signature{
    CentralFileHeader       =   0x02014b50,
    LocalFileHeader         =   0x04034b50,
    DigitalHeader           =   0x05054b50,
    EndOfCentralDirRecord   =   0x06054b50,
    Zip64EndOfCentralDir    =   0x06064b50,
    Zip64EndOfCentralDirLocator=0x07064b50,
    ArchiveExtraData        =   0x08064b50
};

enum VersionMadeBy{
    MS_DOS_AND_OS_2 =0,
    Amiga           =1,
    OpenVMS         =2,
    Unix            =3,
    VM_OR_CMS       =4,
    Atari_ST        =5,
    OS_2_H_P_F_S_   =6,
    Macintosh       =7,
    Z_System        =8,
    CP_M            =9,
    Windows_NTFS    =10,
    MVS             =11,
    VSE             =12,
    Acorn_Risc      =13,
    VFAT            =14,
    Alternate_MVS   =15,
    BeOS            =16,
    Tandem          =17,
    OS_400          =18,
    OS_X            =19,
    other           =20//unused 20~255
};

enum ExtraFieldHeaderID{
    ZIP64           =0x0001,
    AV              =0x0007,
    UnicodeReserved =0x0008,
    OS_2            =0x0009,
    NTFS            =0x000a,
    OpenVMS_ID      =0x000c,
    Unix_ID         =0x000d,
    FileStreamAndForkDescriptorReserved=0x000e,
    PatchDescriptor =0x000f,
    PKCS7Store      =0x0014,
    X509AndIndividualFile=0x0015,
    X509CertificateIDForCentralDir=0x0016,
    StringEncryptionHeader=0x0017,
    RecordManagementControls=0x0018,
    PKCS7EncryptionRecipientCertificateList=0x0019,
    IBMAttributes   =0x0065,
    IBMReserved     =0x0066,
    ZipItMacintosh  =0x2605,
    ZipItMacintoshFile=0x2705,
    ZipItMacintoshDir=0x2805
};


class DataDescriptor{
    struct DescriptorData{
        uint32_t crc_32;
        uint32_t compressed_size;
        uint32_t uncompressed_size;
    };
public:
    DescriptorData data;
    DataDescriptor() = default;
    DataDescriptor(fstream& file){
        file.read((char*)&data, sizeof(data));
    }
};

class FileZip{
    class LocalFileHeader{
        //如此设计是因为内存对齐，如果使用uint32_t则会对齐为32字节
        struct FileHeader{
            uint16_t local_file_header_signature_low;
            uint16_t local_file_header_signature_high;
            uint16_t version_needed_to_extract;
            uint16_t general_purpose_bit_flag;
            uint16_t compression_method;
            uint16_t last_mod_file_time;
            uint16_t last_mod_file_date;
            uint16_t crc_32_low;
            uint16_t crc_32_high;
            uint16_t compressed_size_low;
            uint16_t compressed_size_high;
            uint16_t uncompressed_size_low;
            uint16_t uncompressed_size_high;
            uint16_t file_name_length;
            uint16_t extra_field_length;
        };
    public:
        FileHeader head;
        uint8_t* file_name{nullptr};
        uint8_t* extra_field{nullptr};
        LocalFileHeader()=default;
        LocalFileHeader(std::fstream& file){
            file.read((char*)&head,sizeof(head));

            file_name=new uint8_t[head.file_name_length];
            for(int i=0;i<head.file_name_length;i++){
                file_name[i]=(uint8_t)file.get();
            }

            extra_field = new uint8_t[head.extra_field_length];
            for(int i=0;i<head.extra_field_length;i++){
                extra_field[i]=(uint8_t)file.get();
            }
        }
        ~LocalFileHeader(){
            if(file_name) delete [] file_name;
            if(extra_field) delete [] extra_field;
        }
    };
public:
    shared_ptr<LocalFileHeader> _local_file_header{nullptr};
    uint8_t *_file_data{nullptr};
    shared_ptr<DataDescriptor> _data_descriptor{nullptr};
    FileZip() = default;
    FileZip(fstream& file){
        _local_file_header=make_shared<LocalFileHeader>(file);

        _data_descriptor = make_shared<DataDescriptor>(file);
    }
    ~FileZip(){
        if(_file_data) delete [] _file_data;
    }
};

//EFS
class ArchiveDecryptionHeader{
public:
};

//EFS
class ArchiveExtraDataRecord{
    struct FixedData{
        uint32_t _archive_extra_data_signature;//一般为0x08064b50
        uint32_t _extra_field_length;
    };
public:
    FixedData _fixed_data;
    uint8_t* _extra_field_data{nullptr};
    ArchiveExtraDataRecord() = default;
    ArchiveExtraDataRecord(fstream& file){
        file.read((char*)&_fixed_data, sizeof(_fixed_data));

        _extra_field_data = new uint8_t[_fixed_data._extra_field_length];
        file.read((char*)_extra_field_data, _fixed_data._extra_field_length);
    }
    ~ArchiveExtraDataRecord(){
        if(_extra_field_data) delete [] _extra_field_data;
    }
};

class CentralDirectoryFileHeader{
    struct CentralFileHeader{
        uint16_t _central_file_header_signature_low;
        uint16_t _central_file_header_signature_high;
        uint16_t _version_made_by;
        uint16_t _version_needed_to_extract;
        uint16_t _general_purpose_bit_flag;
        uint16_t _compression_method;
        uint16_t _last_mod_file_time;
        uint16_t _last_mod_file_date;
        uint16_t _crc_32_low;
        uint16_t _crc_32_high;
        uint16_t _compressed_size_low;
        uint16_t _compressed_size_high;
        uint16_t _uncompressed_size_low;
        uint16_t _uncompressed_size_high;
        uint16_t _file_name_length;
        uint16_t _extra_field_length;
        uint16_t _file_comment_length;
        uint16_t _disk_number_start;
        uint16_t _internal_file_attributes;
        uint16_t _external_file_attributes_low;
        uint16_t _external_file_attributes_high;
        uint16_t _relative_offset_of_local_header_low;
        uint16_t _relative_offset_of_local_header_high;
    };
public:
    CentralFileHeader _file_header;
    uint8_t* _file_name{nullptr};
    uint8_t* _extra_field{nullptr};
    uint8_t* _file_comment{nullptr};
    CentralDirectoryFileHeader() = default;
    CentralDirectoryFileHeader(fstream& file){
        file.read((char*)&_file_header, sizeof(_file_header));

        _file_name = new uint8_t[_file_header._file_name_length];
        file.read((char*)_file_name, _file_header._file_name_length);

        _extra_field = new uint8_t[_file_header._extra_field_length];
        file.read((char*)_extra_field, _file_header._extra_field_length);

        _file_comment = new uint8_t[_file_header._file_comment_length];
        file.read((char*)_file_comment, _file_header._file_comment_length);
    }
    ~CentralDirectoryFileHeader(){
        if(_file_name) delete [] _file_name;
        if(_extra_field) delete [] _extra_field;
        if(_file_comment) delete [] _file_comment;
    }
};

class CentralDirectory{
    class DigitalSignature{
        struct DigitalSignatureHead{
            uint16_t _header_signature_low;
            uint16_t _header_signature_high;
            uint16_t _size_of_data;
        };
    public:
        DigitalSignatureHead head;
        uint8_t* _signature_data{nullptr};
        DigitalSignature() = default;
        DigitalSignature(fstream& file){
            file.read((char*)&head, sizeof(head));

            _signature_data = new uint8_t[head._size_of_data];
            file.read((char*)_signature_data, head._size_of_data);
        }
        ~DigitalSignature(){
            if(_signature_data) delete [] _signature_data;
        }
    };
public:
    list<CentralDirectoryFileHeader> _file_head;
    shared_ptr<DigitalSignature> _digital_signature{nullptr};
    CentralDirectory() = default;
    CentralDirectory(fstream& file){
        
        _digital_signature = make_shared<DigitalSignature>(file);
    }
};

class Zip64EndOfCentralDirectoryRecord{
    struct Zip64EndOfCentralDir{
        uint32_t _zip64_end_of_central_dir_signature;
        uint64_t _size_of_zip64_end_of_central_directory_record;
        uint16_t _version_made_by;
        uint16_t _version_needed_to_extract;
        uint32_t _number_of_this_disk;
        uint32_t _number_of_the_disk_with_the_start_of_the_central_directory;
        uint64_t _total_number_of_entries_in_the_central_directory_on_this_disk;
        uint64_t _total_number_of_entries_in_the_central_directory;
        uint64_t _size_of_the_central_directory;
        uint64_t _offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number;
    };
public:
    uint8_t* _zip64_extensible_data_sector{nullptr};
};

class Zip64EndOfCentralDirectoryLocator{
    struct Zip64EndOfCentralDir{
        uint32_t _zip64_end_of_central_dir_locator_signature;
        uint32_t _number_of_the_disk_with_the_start_of_the_zip64_end_of_central_directory;
        uint32_t _relative_offset_of_the_zip64_end_of_central_directory_record_low;
        uint32_t _relative_offset_of_the_zip64_end_of_central_directory_record_high;
        uint32_t _total_number_of_disks;
    };
public:
    Zip64EndOfCentralDir locator;
    Zip64EndOfCentralDirectoryLocator() = default;
    Zip64EndOfCentralDirectoryLocator(fstream& file){
        file.read((char*)&locator, sizeof(locator));
    }
};

class EndOfCentralDirectoryRecord{
    struct EndOfCentralDir{
        uint16_t _end_of_central_dir_signature_low;
        uint16_t _end_of_central_dir_signature_high;
        uint16_t _number_of_this_disk;
        uint16_t _number_of_the_disk_with_the_start_of_the_central_directory;
        uint16_t _total_number_of_entries_in_the_central_directory_on_this_disk;
        uint16_t _total_number_of_entries_in_the_central_directory;
        uint16_t _size_of_the_central_directory_low;
        uint16_t _size_of_the_central_directory_high;
        uint16_t _offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number_low;
        uint16_t _offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number_high;
        uint16_t _dot_zip_file_comment_length;
    };
public:
    EndOfCentralDir dir;
    uint8_t* _dot_zip_file_comment{nullptr};
    EndOfCentralDirectoryRecord()=default;
    EndOfCentralDirectoryRecord(fstream& file){
        file.read((char*)&dir, sizeof(dir));
        _dot_zip_file_comment = new uint8_t[dir._dot_zip_file_comment_length];
        file.read((char*)_dot_zip_file_comment, dir._dot_zip_file_comment_length);
    }
    ~EndOfCentralDirectoryRecord(){
        if(_dot_zip_file_comment) delete [] _dot_zip_file_comment;
    }
};


struct ExtraFieldTagHead{
    uint16_t tag;
    uint16_t size;
};

struct ExtraFieldHead{
    uint16_t type;
    uint16_t tsize;
};

//extra field包含的结构
struct ExtraFieldZIP64{
    uint16_t type;
    uint16_t size;
    uint64_t originalSize;
    uint64_t compressedSize;
    uint64_t RelativeHeaderOffset;
    uint32_t DiskStartNumber;
};

class ExtraFieldOS2{
    struct ExtraFieldOS2Struct{
        uint16_t type;
        uint16_t tsize;
        uint16_t bsize_low;
        uint16_t bsize_high;
        uint16_t ctype;
        uint16_t eacrc_low;
        uint16_t eacrc_high;
    };
public:
    ExtraFieldOS2Struct head;
    uint8_t* compressed_block{nullptr};
    ExtraFieldOS2() = default;
    ExtraFieldOS2(fstream& file){
        file.read((char*)&head, sizeof(head));

        compressed_block = new uint8_t[head.tsize];
        file.read((char*)compressed_block, head.tsize);
    }
    ~ExtraFieldOS2(){
        if(compressed_block) delete [] compressed_block;
    }
};

class ExtraFieldNTFS{
    struct NTFSTag{
        uint16_t tag;
        uint16_t sizeN;
        uint32_t mtime_low;
        uint32_t mtime_high;
        uint32_t atime_low;
        uint32_t atime_high;
        uint32_t ctime_low;
        uint32_t ctime_high;
    };
    struct NTFSHead{
        uint16_t type;
        uint16_t tsize;
        uint16_t reserved_low;
        uint16_t reserved_high;
    };
public:
    NTFSHead head;
    list<NTFSTag> list;
    ExtraFieldNTFS() = default;
    ExtraFieldNTFS(fstream& file){
        file.read((char*)&head, sizeof(head));
        int count=(head.tsize-sizeof(head))/sizeof(NTFSTag);
        while(count--){
            NTFSTag t;
            file.read((char*)&t, sizeof(t));
            list.emplace_back(t);
        }
    }
};

class ExtraFieldOpenVMS{
    class VMSTag{
    public:
        ExtraFieldTagHead tagHead;
        uint8_t* attributeData{nullptr};
        VMSTag() = default;
        VMSTag(fstream& file){
            file.read((char*)&tagHead, sizeof(tagHead));
            attributeData = new uint8_t[tagHead.size];
            file.read((char*)attributeData, tagHead.size);
        }
        ~VMSTag(){
            if(attributeData) delete [] attributeData;
        }
    };
    struct VMSHead{
        uint16_t type;
        uint16_t tsize;
        uint32_t crc;
    };
public:
    VMSHead head;
    list<VMSTag*> lists;
    ExtraFieldOpenVMS() = default;
    ExtraFieldOpenVMS(fstream& file){
        file.read((char*)&head, sizeof(head));
        int surplusSize=head.tsize-sizeof(head);
        while(surplusSize>0){
            VMSTag *t = new VMSTag(file);
            lists.emplace_back(t);
            surplusSize-=t->tagHead.size-sizeof(t->tagHead);
        }
    }
    ~ExtraFieldOpenVMS(){
        for(auto it=lists.begin();it!=lists.end();it++){
            if((*it)!=nullptr){
                delete *it;
            }
        }
    }
};

class ExtraFieldUNIX{
    struct UNIXHead{
        uint16_t type;
        uint16_t tsize;
        uint32_t atime;
        uint32_t mtime;
        uint16_t uid;
        uint16_t gid;
    };
public:
    UNIXHead head;
    uint8_t* data;
    ExtraFieldUNIX() = default;
    ExtraFieldUNIX(fstream& file){
        file.read((char*)&head, sizeof(head));
        data=new uint8_t[head.tsize];
        file.read((char*)data,head.tsize);
    }
    ~ExtraFieldUNIX(){
        if(data) delete [] data;
    }
};

class ExtraFieldPatch{
    struct PatchHead{
        uint16_t type;
        uint16_t tsize;
        uint16_t version;
        uint16_t flags_low;
        uint16_t flags_high;
        uint16_t oldSize_low;
        uint16_t oldSize_high;
        uint16_t oldCRC_low;
        uint16_t oldCRC_high;
        uint16_t newSize_low;
        uint16_t newSize_high;
        uint16_t newCRC_low;
        uint16_t newCRC_high;
    };
public:
    PatchHead head;
    ExtraFieldPatch() = default;
    ExtraFieldPatch(fstream& file){
        file.read((char*)&head, sizeof(head));
    }
};


class ExtraFieldPKCS7Store{
public:
    ExtraFieldHead head;
    uint8_t* tdata;
    ExtraFieldPKCS7Store() = default;
    ExtraFieldPKCS7Store(fstream& file){
        file.read((char*)&head, sizeof(head));
        tdata = new uint8_t[head.tsize];
        file.read((char*)tdata, head.tsize);
    }
    ~ExtraFieldPKCS7Store(){
        if(tdata) delete [] tdata;
    }
};

class ExtraFieldX509AndIndividualFile{
public:
    ExtraFieldHead head;
    uint8_t* tdata;
    ExtraFieldX509AndIndividualFile() = default;
    ExtraFieldX509AndIndividualFile(fstream& file){
        file.read((char*)&head, sizeof(head));
        tdata = new uint8_t[head.tsize];
        file.read((char*)tdata, head.tsize);
    }
    ~ExtraFieldX509AndIndividualFile(){
        if(tdata) delete [] tdata;
    }
};

class ExtraFieldX509CertificateIDForCentralDir{
public:
    ExtraFieldHead head;
    uint8_t* tdata;
    ExtraFieldX509CertificateIDForCentralDir() = default;
    ExtraFieldX509CertificateIDForCentralDir(fstream& file){
        file.read((char*)&head, sizeof(head));
        tdata = new uint8_t[head.tsize];
        file.read((char*)tdata, head.tsize);
    }
    ~ExtraFieldX509CertificateIDForCentralDir(){
        if(tdata) delete [] tdata;
    }
};

class ExtraFieldStringEncryptionHeader{
    struct StringEncryptionHeader{
        uint16_t type;
        uint16_t tsize;
        uint16_t format;
        uint16_t algid;
        uint16_t bitlen;
        uint16_t flags;
    };
public:
    StringEncryptionHeader head;
    uint8_t* certData{nullptr};
    ExtraFieldStringEncryptionHeader() = default;
    ExtraFieldStringEncryptionHeader(fstream& file){
        file.read((char*)&head, sizeof(head));
        certData=new uint8_t[head.tsize];
        file.read((char*)certData, head.tsize);
    }
    ~ExtraFieldStringEncryptionHeader(){
        if(certData) delete [] certData;
    }
};

class ExtraFieldRecordManagementControls{
    class RecCTLTag{
    public:
        ExtraFieldTagHead head;
        uint8_t* data{nullptr};
        RecCTLTag() = default;
        RecCTLTag(fstream& file){
            file.read((char*)&head, sizeof(head));
            data=new uint8_t[head.size];
            file.read((char*)data, sizeof(head.size));
        }
        ~RecCTLTag(){
            if(data) delete [] data;
        }
    };
public:
    ExtraFieldHead head;
    list<RecCTLTag*> lists;
    ExtraFieldRecordManagementControls() = default;
    ExtraFieldRecordManagementControls(fstream& file){
        file.read((char*)&head, sizeof(head));
        int surplus=head.tsize-sizeof(head);
        while(surplus>0){
            RecCTLTag* t=new RecCTLTag(file);
            lists.emplace_back(t);
            surplus-=t->head.size-sizeof(t->head);
        }
    }
    ~ExtraFieldRecordManagementControls(){
        for(auto it=lists.begin();it!=lists.end();it++){
            if((*it)!=nullptr){
                delete *it;
            }
        }
    }
};

class ExtraFieldEFS{
public:
    ExtraFieldHead head;
    uint16_t version;
    uint8_t* cStore{nullptr};
    ExtraFieldEFS() = default;
    ExtraFieldEFS(fstream& file){
        file.read((char*)&head, sizeof(head));
        file.read((char*)&version, sizeof(version));
        int storeSize = head.tsize-sizeof(version);
        cStore = new uint8_t[storeSize];
        file.read((char*)cStore, storeSize);
    }
    ~ExtraFieldEFS(){
        if(cStore) delete [] cStore;
    }
};

class ExtraFieldMVS{
    struct MVSHead : public ExtraFieldHead{
        uint32_t ID;
    };
public:
    MVSHead head;
    uint8_t* data{nullptr};
    ExtraFieldMVS(fstream& file){
        file.read((char*)&head, sizeof(head));
        data = new uint8_t[head.tsize];
        file.read((char*)data, head.tsize);
    }
    ~ExtraFieldMVS(){
        if(data) delete [] data;
    }
};

class ExtraFieldIBMAttributes{
    struct IBMAttributesHead : public ExtraFieldHead{
        uint32_t ID;
    };
public:
    IBMAttributesHead head;
    uint8_t* data{nullptr};
    ExtraFieldIBMAttributes(fstream& file){
        file.read((char*)&head, sizeof(head));
        data = new uint8_t[head.tsize];
        file.read((char*)data, head.tsize);
    }
    ~ExtraFieldIBMAttributes(){
        if(data) delete [] data;
    }
};

//Mac2
class ExtraFieldZipItMacintosh{
    struct ZipItMacintoshHead : public ExtraFieldHead{
        uint32_t zpit;
    };
public:
    ZipItMacintoshHead head;
    uint8_t fileNameLen;
    uint8_t* fileName{nullptr};
    uint8_t fileType[4];
    uint8_t creator[4];
    ExtraFieldZipItMacintosh()=default;
    ExtraFieldZipItMacintosh(fstream& file){
        file.read((char*)&head, sizeof(head));
        file.read((char*)&fileNameLen, sizeof(fileNameLen));
        fileName=new uint8_t[fileNameLen];
        file.read((char*)fileName, fileNameLen);
        file.read((char*)fileType, sizeof(uint8_t)*4);
        file.read((char*)creator, sizeof(uint8_t)*4);
    }
    ~ExtraFieldZipItMacintosh(){
        if(fileName) delete [] fileName;
    }
};

//Mac2b
class ExtraFieldZipItMacintoshFile{
    struct ZipItMacintoshFileHead : public ExtraFieldHead{
        uint32_t zpit;
    };
public:
    ZipItMacintoshFileHead head;
    uint8_t fileType[4];
    uint8_t creator[4];
    uint16_t fdFlags;
    uint16_t reserved{0};
    ExtraFieldZipItMacintoshFile() = default;
    ExtraFieldZipItMacintoshFile(fstream& file){
        file.read((char*)&head, sizeof(head));
        file.read((char*)fileType, sizeof(uint8_t)*4);
        file.read((char*)creator, sizeof(uint8_t)*4);
        file.read((char*)&fdFlags, sizeof(fdFlags));
        file.read((char*)&reserved, sizeof(reserved));
    }
};

//Mac2c
class ExtraFieldZipItMacintoshDir{
    struct ZipItMacintoshDirHead : public ExtraFieldHead{
        uint32_t zpit;
        uint16_t frFlags;
        uint16_t view;
    };
public:
    ZipItMacintoshDirHead head;
    ExtraFieldZipItMacintoshDir() = default;
    ExtraFieldZipItMacintoshDir(fstream& file){
        file.read((char*)&head, sizeof(head));
    }
};

class ZIP{
public:
    list<FileZip> _file_zip;
    ArchiveDecryptionHeader _archive_decryption_header;
    ArchiveExtraDataRecord _archive_extra_data_record;
    CentralDirectory _central_directory;
    ZIP(const string& fileName){
        fstream file(fileName,ios::in|ios::binary);
    }
    ~ZIP(){
    }
};

NAME_SPACE_END()

#endif //!_COMPRESS_