//
// Created by Administrator on 2025/5/4.
//

#include "server/include/log_server.h"

#include <iostream>

#define MAX_CONNECT 128

namespace myUtil {

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
        std::cerr << "Connection error: " << uv_strerror(status) << std::endl;
        return;
    }
    std::cout << "Connection established." << std::endl;
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
        printf("%.*s\n", (int)nRead, buf->base);
    } else if (nRead < 0) {
        std::cerr << "Read error: " << uv_strerror(static_cast<int>(nRead)) << std::endl;
        uv_close((uv_handle_t*)client, OnClose);
    }
    delete[] buf->base;
}
} // namespace myUtil
