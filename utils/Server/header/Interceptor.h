#pragma once
#include "Net.h"
#include "Reflex.h"
using namespace myUtil;
#ifndef _INTERCEPTOR_
#define _INTERCEPTOR_

class Interceptor : public RObject {
public:
    virtual bool preHandle(const Request& request, const Response& response) { return true; }

    virtual void postHandle(const Request& request, const Response& response) {}

    virtual void afterCompletion(const Request& request, const Response& response) {}
};

#endif //!_INTERCEPTOR_