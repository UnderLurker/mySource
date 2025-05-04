//
// Created by 常笑男 on 2025/5/2.
//

#ifndef _M4A_ENUM_TYPE_H
#define _M4A_ENUM_TYPE_H

#include <cstdint>

namespace myUtil {

#define CONVERT(a) (((int32_t)(#a[0]) << 24) + ((int32_t)(#a[1]) << 16) + ((int32_t)(#a[2]) << 8) + (int32_t)(#a[3]))

enum BoxType : uint32_t {
    ASSP = CONVERT(assp),
    BXML = CONVERT(bxml),
    CO64 = CONVERT(co64),
    CPRT = CONVERT(cprt),
    CSLG = CONVERT(cslg),
    CTTS = CONVERT(ctts),
    DINF = CONVERT(dinf),
    DREF = CONVERT(dref),
    EDTS = CONVERT(edts),
    ELNG = CONVERT(elng),
    ELST = CONVERT(elst),
    FREE = CONVERT(free),
    FTYP = CONVERT(ftyp),
    HDLR = CONVERT(hdlr),
    IINF = CONVERT(iinf),
    ILOC = CONVERT(iloc),
    INFE = CONVERT(infe),
    IPRO = CONVERT(ipro),
    KIND = CONVERT(kind),
    LEVA = CONVERT(leva),
    MDAT = CONVERT(mdat),
    MDHD = CONVERT(mdhd),
    MDIA = CONVERT(mdia),
    MECO = CONVERT(meco),
    MEHD = CONVERT(mehd),
    META = CONVERT(meta),
    MFHD = CONVERT(mfhd),
    MFRA = CONVERT(mfra),
    MFRO = CONVERT(mfro),
    MINF = CONVERT(minf),
    MOOF = CONVERT(moof),
    MOOV = CONVERT(moov),
    MVEX = CONVERT(mvex),
    MVHD = CONVERT(mvhd),
    NMHD = CONVERT(nmhd),
    PADB = CONVERT(padb),
    PDIN = CONVERT(pdin),
    PITM = CONVERT(pitm),
    SAIO = CONVERT(saio),
    SAIZ = CONVERT(saiz),
    SBGP = CONVERT(sbgp),
    SDTP = CONVERT(sdtp),
    SGPD = CONVERT(sgpd),
    SKIP = CONVERT(skip),
    STBL = CONVERT(stbl),
    STCO = CONVERT(stco),
    STDP = CONVERT(stdp),
    STSC = CONVERT(stsc),
    STSD = CONVERT(stsd),
    STSH = CONVERT(stsh),
    STSS = CONVERT(stss),
    STSZ = CONVERT(stsz),
    STTS = CONVERT(stts),
    STZ2 = CONVERT(stz2),
    SUBS = CONVERT(subs),
    TFDT = CONVERT(tfdt),
    TFHD = CONVERT(tfhd),
    TFRA = CONVERT(tfra),
    TKHD = CONVERT(tkhd),
    TRAF = CONVERT(traf),
    TRAK = CONVERT(trak),
    TREF = CONVERT(tref),
    TREP = CONVERT(trep),
    TREX = CONVERT(trex),
    TRGR = CONVERT(trgr),
    TRUN = CONVERT(trun),
    TSEL = CONVERT(tsel),
    UDTA = CONVERT(udta),
    URL  = CONVERT(url),
    URN  = CONVERT(urn),
    UUID = CONVERT(uuid),
    VMHD = CONVERT(vmhd),
    XML  = CONVERT(xml),
};

} // namespace myUtil

#endif // _M4A_ENUM_TYPE_H
