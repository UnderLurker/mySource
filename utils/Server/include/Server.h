#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <string>
#include <unordered_map>

#include "Ini.h"
#include "Net.h"
#include "Util.h"

NAME_SPACE_START(myUtil)
using namespace std;
#include <winsock.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080

class Server {
public:
    Server();
    explicit Server(const std::string& addr = SERVER_ADDR, const int& port = SERVER_PORT);
    ~Server() = default;

public:
    bool listen(const int& maxConnect = 1);
    void setRoute(const string& url, const string& className, const string& classFunName);
    void runRoute(const Request& req, Response* resp);
    void setInterceptor(const string& url, const string& InterceptorName);
    void close() const;

protected:
    static bool Init();
    void threadFunc(SOCKET m_server);
    static int sendTelegram(const SOCKET& accept, const string& info, int flags);

private:
    SOCKET m_server {};
    SOCKADDR_IN m_add_in {};
    // thread listenThread;
    int connectCount {0};
    unordered_map<string, pair<string, string>> routeMap;
    unordered_map<string, string> interceptorMap;
    IniHelper iniHelper;
};

NAME_SPACE_END()
#endif //!_SERVER_H_
