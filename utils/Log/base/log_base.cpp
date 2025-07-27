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

uv_loop_t* LogBase::_loop = nullptr;
uv_tcp_t LogBase::_tcp;
sockaddr_in LogBase::_addr;
uv_write_t LogBase::_write_req;

LogBase::LogBase(const std::string& ip, int32_t port) {
    _loop = uv_default_loop();
    uv_tcp_init(_loop, &_tcp);
    uv_ip4_addr(ip.c_str(), port, &_addr);
}

LogBase::~LogBase() {
    uv_loop_close(_loop);
    CloseTcp();
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
        printf("Read error: %s\n", uv_strerror(static_cast<int>(nRead)));
        uv_read_stop(client);
        uv_close((uv_handle_t*)client, OnClose);
    }
    delete[] buf->base;
}

void LogBase::OnClose(uv_handle_t* handle) {
    printf("logBase::OnClose\n");
}

void LogBase::Recv(uv_stream_t* stream) { uv_read_start(stream, AllocBuffer, OnRead); }

void LogBase::CloseTcp() {
    printf("logBase::CloseTcp\n");
    uv_read_stop((uv_stream_t*)&_tcp);
    uv_close((uv_handle_t*)&_tcp, OnClose);
}

void LogBase::AllocBuffer(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf) {
    buf->base = new char[suggestedSize];
    buf->len  = suggestedSize;
}

std::string LogBase::GetSockaddr(const sockaddr* addr) {
    if (!addr) return "";
    char ip[INET6_ADDRSTRLEN];
    int port;

    if (addr->sa_family == AF_INET) {
        sockaddr_in* addr_in = (sockaddr_in*)addr;
        inet_ntop(AF_INET, &(addr_in->sin_addr), ip, sizeof(ip));
        port = ntohs(addr_in->sin_port);
    } else if (addr->sa_family == AF_INET6) {
        sockaddr_in6* addr_in6 = (sockaddr_in6*)addr;
        inet_ntop(AF_INET6, &(addr_in6->sin6_addr), ip, sizeof(ip));
        port = ntohs(addr_in6->sin6_port);
    } else {
        return "Unknown address family\n";
    }
    std::string result = "Address: ";
    result.append(ip);
    result.append(", Port: ");
    result.append(std::to_string(port));
    result.append("\n");
    return result;
}
} // namespace myUtil
