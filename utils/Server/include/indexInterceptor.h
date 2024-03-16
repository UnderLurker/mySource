#ifndef _INDEX_INTERCEPTOR_H
#define _INDEX_INTERCEPTOR_H
#include "Interceptor.h"

NAME_SPACE_START(myUtil)
class IndexInterceptor : public Interceptor {
public:
    bool preHandle(const Request& request, const Response& response) override { return false; }

    void postHandle(const Request& request, const Response& response) override {}

    void afterCompletion(const Request& request, const Response& response) override {}
};
NAME_SPACE_END()
#endif //!_INDEX_INTERCEPTOR_H
