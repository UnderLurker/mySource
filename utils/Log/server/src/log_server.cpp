//
// Created by Administrator on 2025/5/4.
//

#include "server/include/log_server.h"

#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>

#define MAX_CONNECT 128

namespace myUtil {
namespace {
std::string GetTime() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) % 1000;
 
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    struct tm *tm_info = std::localtime(&now_time);
 
    char timestamp[64];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
 
    std::ostringstream oss;
    oss << timestamp << "." << std::setfill('0') << std::setw(3) << ms.count();
 
    return oss.str();
}
}

uv_loop_t* LogBase::_loop = nullptr;
uv_tcp_t LogBase::_tcp;
sockaddr_in LogBase::_addr;
uv_write_t LogBase::_write_req;

LogServer::LogServer(const std::string& ip, int32_t port)
    : LogBase(ip, port) {
    uv_tcp_bind(&_tcp, reinterpret_cast<const sockaddr*>(&_addr), 0);
    uv_listen((uv_stream_t*)&_tcp, MAX_CONNECT, OnConnect);
}

void LogServer::OnConnect(uv_stream_t* server, int status) {
    if (status < 0) {
        printf("Connection error: %s\n", uv_strerror(status));
        return;
    }
    sockaddr_storage peerAddr;
    int32_t addrLen = sizeof(peerAddr);
    uv_tcp_getpeername((uv_tcp_t*)server, (sockaddr*)&peerAddr, &addrLen);
    printf("Connection established. Info: %s\n", LogBase::GetSockaddr((sockaddr*)&peerAddr).c_str());
    auto* client = new uv_tcp_t();
    uv_tcp_init(_loop, client);
    OnAccept(server, (uv_stream_t*)client);
}

void LogServer::OnAccept(uv_stream_t* server, uv_stream_t* client) {
    if (uv_accept(server, client) == 0) {
        uv_read_start((uv_stream_t*)client, AllocBuffer, OnRead);
    } else {
        uv_close((uv_handle_t*)client, OnClose);
    }
}

void LogServer::OnRead(uv_stream_t* client, ssize_t nRead, const uv_buf_t* buf) {
    if (nRead > 0) {
        char* data = buf->base;
        std::string_view strView(data, strlen(data)), next;
        auto decodePos = Decode(strView, next);
        if (decodePos) {
            printf("%s %s\n", GetTime().c_str(), std::string(strView).substr(0, decodePos).c_str());
        }
        while(!next.empty()) {
            std::string_view tmp;
            decodePos = Decode(next, tmp);
            if (decodePos) {
                printf("%s %s\n", GetTime().c_str(), std::string(next).substr(0, decodePos).c_str());
            }
            std::swap(next, tmp);
        }
    } else if (nRead < 0) {
        printf("%s Read error: %s\n", GetTime().c_str(), uv_strerror(static_cast<int>(nRead)));
        uv_close((uv_handle_t*)client, OnClose);
    }
    delete[] buf->base;
}

uint32_t LogServer::Decode(std::string_view& data, std::string_view& next) {
    if (data.empty()) return 0;
    int32_t start = 0;
    if (data[start] < '0' || data[start] > '9') return 0;
    int32_t end = start + 1 + data[start] - '0';
    int32_t len = 0;
    for (int32_t i = start + 1; i < end; i++) {
        len = len * 10 + data[i] - '0';
    }
    if (end >= data.size()) return 0;
    std::string_view nextData(data.data() + end);
    data.swap(nextData);
    if (len < data.size()) {
        next = std::string_view(data.data() + len);
    }
    return len;
}
} // namespace myUtil
