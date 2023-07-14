#include "Util.h"
#include "Compress.h"
#include <cstdint>
#include <exception>
#include <stdint.h>
NAME_SPACE_START(myUtil)

void ZIP::ProcessStart(){
    try{
        while(!file.eof()){
            uint32_t signature=0;
            uint16_t tagSignature=0;
            file.read((char*)&signature, sizeof(uint32_t));
            file.seekg(-4,ios::cur);
            cout<<file.tellg()<<endl;
            switch (signature) {
            case CentralFileHeader:{
                _central_directory = new CentralDirectory(file);
                break;
            }
            case LocalFileHeader:{
                FileZip* fz=new FileZip(file);
                _file_zip.emplace_back(fz);
                break;
            }
            // case DigitalHeader:{
            //     break;
            // }
            case EndOfCentralDirRecord:{
                _end_of_central_dir_record = new EndOfCentralDirectoryRecord(file);
                break;
            }
            case Zip64EndOfCentralDir:{
                _zip64_end_of_central_dir_record = new Zip64EndOfCentralDirectoryRecord(file);
                break;
            }
            case Zip64EndOfCentralDirLocator:{
                _zip64_end_of_central_dir_locator = new Zip64EndOfCentralDirectoryLocator(file);
                break;
            }
            case ArchiveExtraData:{
                _archive_extra_data_record= new ArchiveExtraDataRecord(file);
                break;
            }
            default:
                cout<<hex<<signature<<endl;
                break;
            }
        }
    }
    catch(exception ex){

    }
}

NAME_SPACE_END()