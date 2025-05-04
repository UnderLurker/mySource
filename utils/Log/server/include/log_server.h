//
// Created by Administrator on 2025/5/4.
//

#ifndef _LOG_SERVER_H
#define _LOG_SERVER_H

#include "base/log_base.h"

namespace myUtil {

class LogServer : public LogBase {
public:
    explicit LogServer(const std::string& ip, int32_t port);

protected:
    static void OnConnect(uv_stream_t* server, int status);
    static void OnAccept(uv_stream_t* server, uv_stream_t* client);
    static void OnRead(uv_stream_t* client, ssize_t nRead, const uv_buf_t* buf);
};

} // namespace myUtil

#endif // _LOG_SERVER_H
