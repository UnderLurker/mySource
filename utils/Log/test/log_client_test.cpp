//
// Created by Administrator on 2025/5/4.
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <uv.h>

#include "client/include/log_client.h"

int main() {
    auto client = myUtil::LogClient::GetInstance();
    client->Send("hello");
    client->Send("hello1");
    client->Send("hello2");
    client->Send("hello3");
    client->Send("hello4");
    client->Send("hello5");
    client->Send("hello6");
    client->Send("hello7");
    client->Send("hello8");
    delete client;
    return 0;
}
