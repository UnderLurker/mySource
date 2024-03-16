#ifndef _INTERCEPTOR_H
#define _INTERCEPTOR_H

#include "Net.h"
#include "Reflex.h"

NAME_SPACE_START(myUtil)

class Interceptor : public RObject {
public:
    virtual bool preHandle(const Request& request, const Response& response) { return true; }

    virtual void postHandle(const Request& request, const Response& response) {}

    virtual void afterCompletion(const Request& request, const Response& response) {}
};

NAME_SPACE_END()
#endif //!_INTERCEPTOR_H
