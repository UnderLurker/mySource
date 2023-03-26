#pragma once
#include "Interceptor.h"
class IndexInterceptor : public Interceptor {
public:
    bool preHandle(const Request& request, const Response& response) override {
        return false;
    }

    void postHandle(const Request& request, const Response& response) override {}

    void afterCompletion(const Request& request, const Response& response) override {}
};