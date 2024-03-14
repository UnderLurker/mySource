//
// Created by 常笑男 on 2023/8/19.
//

#ifndef _JSON_EXCEPTION_
#define _JSON_EXCEPTION_

#include "Util.h"

NAME_SPACE_START(myUtil)

class JsonException : public exception{
public:
    const char * what() const noexcept override{
        return "Json error";
    }
};

class JsonMismatchException : public exception{
public:
    const char * what() const noexcept override{
        return "Json mismatch error";
    }
};


NAME_SPACE_END()


#endif //_JSON_EXCEPTION_
