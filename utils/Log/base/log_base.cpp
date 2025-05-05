//
// Created by 常笑男 on 2025/5/4.
//
#include "base/log_base.h"

namespace myUtil {
namespace {
void OnWrite(uv_write_t* req, int status) {
    if (status < 0) {
        fprintf(stderr, "Write error: %s\n", uv_strerror(status));
        return;
    }
}
} // namespace

LogBase::LogBase(const std::string& ip, int32_t port) {
    _loop = uv_default_loop();
    uv_tcp_init(_loop, &_tcp);
    uv_ip4_addr(ip.c_str(), port, &_addr);
}

LogBase::~LogBase() {
    uv_loop_close(_loop);
    uv_tcp_close_reset(&_tcp, nullptr);
}

void LogBase::Send(const std::string& message) {
    uv_buf_t buf = uv_buf_init((char*)message.c_str(), message.size());
    int ret      = uv_write(&_write_req, (uv_stream_t*)&_tcp, &buf, 1, OnWrite);
    if (ret < 0) {
        std::cerr << "Send error: " << uv_strerror(ret) << std::endl;
        return;
    }
}

void LogBase::OnRead(uv_stream_t* client, ssize_t nRead, const uv_buf_t* buf) {
    if (nRead > 0) {
        printf("Received data: %.*s\n", (int)nRead, buf->base);
    } else if (nRead < 0) {
        std::cerr << "Read error: " << uv_strerror(static_cast<int>(nRead)) << std::endl;
        uv_close((uv_handle_t*)client, OnClose);
    }
    delete[] buf->base;
}

void LogBase::OnClose(uv_handle_t* handle) {}

void LogBase::Recv(uv_stream_t* stream) { uv_read_start(stream, AllocBuffer, OnRead); }

void LogBase::AllocBuffer(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf) {
    buf->base = new char[suggestedSize];
    buf->len  = suggestedSize;
}
} // namespace myUtil
