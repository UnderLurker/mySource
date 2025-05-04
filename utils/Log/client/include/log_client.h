//
// Created by Administrator on 2025/5/4.
//

#ifndef _LOG_CLIENT_H
#define _LOG_CLIENT_H

#include "base/log_base.h"

#define LOG_CLIENT_PORT 8880

namespace myUtil {

class LogClient : public LogBase {
public:
    static LogClient* GetInstance();
    void Recv();
    void Send(const std::string& message) { LogBase::Send(message); }

protected:
    static void OnConnect(uv_connect_t* req, int status);
    void Run(uv_run_mode mode = uv_run_mode::UV_RUN_DEFAULT) { uv_run(_loop, mode); }

private:
    explicit LogClient(const std::string& ip, int32_t port);

private:
    static uv_connect_t _connect_req;
    static thread_local LogClient* _logClientInstance;
};

} // namespace myUtil

#endif // _LOG_CLIENT_H
