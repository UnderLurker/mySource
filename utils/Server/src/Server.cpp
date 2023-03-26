#include "Server.h"
#include <minwindef.h>
#include <string>
#include <thread>
#include <winsock.h>
#include <iostream>
#include <fstream>
#include "Net.h"
#include "Util.h"
#include "Reflex.h"
#include "CController.h"
#include "Interceptor.h"
using namespace std;

NAME_SPACE_START(myUtil)

Server::Server()
{
    m_add_in.sin_family = AF_INET;
    m_add_in.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    m_add_in.sin_port = htons(SERVER_PORT);
}

Server::Server(const std::string& addr, const int& port)
{
    m_add_in.sin_family = AF_INET;
    m_add_in.sin_addr.S_un.S_addr = inet_addr(addr.c_str());
    m_add_in.sin_port = htons(port);
}

bool Server::listen(const int& maxConnect)
{
    if (!Init()) {
        return false;
    }
    m_server = socket(AF_INET, SOCK_STREAM, 0);
    if (::bind(m_server, (sockaddr*)&m_add_in, sizeof(SOCKADDR)) == SOCKET_ERROR) {
        WSACleanup();
        return false;
    }
    if (::listen(m_server, maxConnect) < 0) {
        WSACleanup();
        return false;
    }
    thread listenThread(&Server::threadFunc, this, m_server);
    listenThread.join();
    return true;
}

void Server::setRoute(const string& url, const string& className, const string& classFunName)
{
    routeMap.insert(pair<string, pair<string, string>>(url, pair<string, string>(className, classFunName)));
}

void Server::runRoute(const Request& req, Response* resp)
{
    string url = req.getRequestStatus().Url;
    Reflex* factory = myUtil::Singleton<Reflex>::Instance();
    string interceptorName = "";
    string res = "";
    string content = "";
    //������
    //��ȥ������ӳ�����Ѱ��������û�еĻ�Ĭ��ʹ�û���
    auto interceptorIt = interceptorMap.find(url);
    if (interceptorIt != interceptorMap.end()) interceptorName = interceptorIt->second;
    Interceptor* inter = (Interceptor*)factory->createClass(interceptorName);
    if (inter == nullptr) inter = new Interceptor();
    if (inter->preHandle(req, *resp)) {
        //����
        auto it = routeMap.find(url);
        if (it != routeMap.end()) {
            CController* cont = (CController*)factory->createClass(it->second.first);
            res = cont->Call<string, Request, Response*>(it->second.second, req, resp);
        }
        //�������
    }
    else {
        resp->setResponseStatus("HTTP", 1, 1, 404, "Forbidden");
    }
    if (url.find("favicon.ico") != string::npos) {
        content = getFile(iniHelper.getIniConfig("staticResource", "favicon_path", "./favicon.ico"));
        resp->setResponseHead("content-type", "image/x-icon");
    }
    else if(res != "") {
        try {
            content = getFile(res);
        }
        catch(exception ex){
            content = ex.what();
        }
    }
    resp->setResponseContent(content);
    auto list = resp->getCookie();
    for (auto item : list) {
        resp->setResponseHead("Set-Cookie", item.toString());
    }
    resp->setResponseHead("content-length", to_string(content.size()));
    resp->setResponseHead("Server", "C++MVC");
    inter->postHandle(req, *resp);
}

void Server::setInterceptor(const string& url, const string& InterceptorName)
{
    interceptorMap.insert(pair<string, string>(url, InterceptorName));
}

void Server::close()
{
    closesocket(m_server);
    WSACleanup();
}

bool Server::Init()
{
    WORD ver = MAKEWORD(2, 2);
    WSADATA wsadata;
    int errFlag = -1;
    errFlag = WSAStartup(ver, &wsadata);
    if (errFlag != 0) return false;
    //���汾��
    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        WSACleanup();
        return false;
    }
    return true;

}
void Server::threadFunc(SOCKET m_server)
{
    while (1) {
        SOCKADDR_IN m_acc_in;
        int len = sizeof(SOCKADDR);
        SOCKET m_accept = accept(m_server, (sockaddr*)&m_acc_in, &len);
        if (m_accept == SOCKET_ERROR) {
            continue;
        }
        int recv_len = 0;
        char recv_buf[10000];
        recv_len = recv(m_accept, recv_buf, 10000, 0);
        
        //char ת wchar
        int  unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, recv_buf, -1, NULL, 0);
        wchar_t* pUnicode = new  wchar_t[unicodeLen];
        memset(pUnicode, 0, unicodeLen * sizeof(wchar_t));
        ::MultiByteToWideChar(CP_UTF8, 0, recv_buf, -1, (LPWSTR)pUnicode, unicodeLen);
        wstring  rt = pUnicode;
        //�����С
        char* pAscii = new char[recv_len];
        memset(pAscii, 0, sizeof(char) * recv_len);
        strncpy(pAscii, recv_buf, recv_len);
        string lrt(pAscii);
        //��������
        Request req(lrt);
        Response resp;
        runRoute(req, &resp);
        cout << "�����ַ��" << req.getRequestStatus().Url << endl;
        sendTelegram(m_accept, resp.toString(), 0);
        closesocket(m_accept);
    }
}

int Server::sendTelegram(const SOCKET& accept, const string& info, int flags)
{
    int res = send(accept, info.c_str(), info.size(), flags);
    return res;
}

NAME_SPACE_END()
