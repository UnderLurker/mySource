#ifndef __MD5__
#define __MD5__

#include "Util.h"

NAME_SPACE_START(myUtil)

//github上找的，我写的我也找不出问题在哪里
class MD5{
public:
    static string encodeFile(const string& filePath);
    static string encodeStr(const string& str);
};

NAME_SPACE_END();

#endif // !__MD5__