//
// Created by Administrator on 2025/5/4.
//
#include <cstdio>
#include <cstdlib>
#include <uv.h>

#include "server/include/log_server.h"

int main() {
    myUtil::LogServer server("0.0.0.0", 8880);
    server.Run();
    return 0;
}
