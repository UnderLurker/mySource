//
// Created by Administrator on 2025/5/4.
//
#include "client/include/log_client.h"

#include <iostream>

namespace myUtil {

uv_loop_t* LogBase::_loop = nullptr;
uv_tcp_t LogBase::_tcp;
sockaddr_in LogBase::_addr;
uv_connect_t LogClient::_connect_req;
uv_write_t LogBase::_write_req;
thread_local LogClient* LogClient::_logClientInstance = nullptr;

LogClient* LogClient::GetInstance() {
    if (_logClientInstance) {
        return _logClientInstance;
    }
    _logClientInstance = new LogClient("127.0.0.1", LOG_CLIENT_PORT);
    _logClientInstance->Run();
    return _logClientInstance;
}

void LogClient::Recv() { LogBase::Recv(reinterpret_cast<uv_stream_t*>(&_tcp)); }

void LogClient::Send(const std::string& message) {
    auto size = message.size();
    int32_t len = 0;
    while(size) {
        size /= 10;
        len++;
    }
    std::string tmp = std::to_string(len) + std::to_string(message.size()) + message;
    LogBase::Send(tmp);
}

void LogClient::OnConnect(uv_connect_t* req, int status) {
    if (status < 0) {
        std::cerr << "Connect error: " << uv_strerror(status) << std::endl;
        return;
    }
    std::cout << "Connected to server.\n";
}

LogClient::LogClient(const std::string& ip, int32_t port)
    : LogBase(ip, port) {
    uv_tcp_connect(&_connect_req, &_tcp, (const struct sockaddr*)&_addr, OnConnect);
}

} // namespace myUtil
