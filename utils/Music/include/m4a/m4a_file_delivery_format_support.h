//
// Created by 常笑男 on 2026/3/14.
//

#ifndef _M4A_FILE_DELIVERY_FORMAT_SUPPORT_H
#define _M4A_FILE_DELIVERY_FORMAT_SUPPORT_H

#include "m4a/m4a_common.h"

#include <memory>

namespace myUtil {
// PAEN
struct PartitionEntry : public Box {};
// FIIN
struct FDItemInformationBox : public FullBox {

};
} // namespace myUtil

#endif // _M4A_FILE_DELIVERY_FORMAT_SUPPORT_H
