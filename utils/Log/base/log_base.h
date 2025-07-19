//
// Created by 常笑男 on 2025/5/4.
//

#ifndef _LOG_BASE_H
#define _LOG_BASE_H

#include <iostream>
#include <uv.h>

namespace myUtil {

class LogBase {
public:
    explicit LogBase(const std::string& ip, int32_t port);
    virtual ~LogBase();
    virtual void Send(const std::string& message);
    void Run(uv_run_mode mode = uv_run_mode::UV_RUN_DEFAULT) { uv_run(_loop, mode); }
    static void OnRead(uv_stream_t* client, ssize_t nRead, const uv_buf_t* buf);
    static void OnClose(uv_handle_t* handle);

protected:
    void Recv(uv_stream_t*);
    void CloseTcp();
    static void AllocBuffer(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf);
    static std::string GetSockaddr(const sockaddr* addr);

protected:
    static uv_loop_t* _loop;
    static uv_tcp_t _tcp;
    static sockaddr_in _addr;
    static uv_write_t _write_req;
};

} // namespace myUtil

#endif // _LOG_BASE_H
