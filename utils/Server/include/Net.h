#ifndef _NET_H
#define _NET_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Cookie.h"
#include "Util.h"
using namespace std;

NAME_SPACE_START(myUtil)

#define BLACK "\r\n"
#define SPACE " "

class Net {
public:
    virtual string toString()                    = 0;
    virtual vector<Cookie> getCookie() const     = 0;
    virtual void setCookie(const Cookie& cookie) = 0;
    Net() = default;
    virtual ~Net() = default;

protected:
    vector<Cookie> _cookie;
};

struct RequestStatus {
    string RMethod;
    string Url;
    string ProName;
    short verHigh;
    short verLow;
};

struct ResponseStatus {
    string ProName;
    short verHigh;
    short verLow;
    short status;
    string statusWord;
};

class Request : public Net {
public:
    Request() = default;
    explicit Request(const string& sourceStr);
    void setRequestStatus(const string& method   = "GET",
                          const string& url      = "/",
                          const string& _proName = "HTTP",
                          const short& _verHigh  = 1,
                          const short& _verLow   = 1);
    void setRequestHead(const string& headKey, const string& headValue);
    void setRequestContent(const string& content);

    RequestStatus getRequestStatus() const;
    string getRequestContent(const string& headKey) const;

    vector<Cookie> getCookie() const override;
    void setCookie(const Cookie& cookie) override;

    string toString() override;
    ~Request() override = default;

private:
    RequestStatus _status;
    unordered_map<string, string> _RequestHead;
    string _RequestContent {""};
};

class Response : public Net {
public:
    Response();
    void setResponseStatus(const string& _proName = "HTTP",
                           const short& _verHigh  = 1,
                           const short& _verLow   = 1,
                           const short& status    = 200,
                           const string& word     = "");
    void setResponseHead(const string& headKey, const string& headValue);
    void setResponseContent(const string& content);

    ResponseStatus getResponseStatus() const;
    string getResponseHeadByKey(const string& headKey) const;

    vector<Cookie> getCookie() const override;
    void setCookie(const Cookie& cookie) override;

    string toString() override;
    ~Response() override = default;

private:
    ResponseStatus _status;
    unordered_map<string, string> _ResponseHead;
    string _ResponseContent;
};

class Analyse {
public:
    static vector<string> getVectorBySplit(const string& source, const char& ch);
    static unordered_map<string, string> getMapBySplit(const string& source, const char& ch1, const char& ch2);
    static string makeVectorByChar(const vector<string>& v, const char& ch);
    static string makeMapByChars(const unordered_map<string, string>& m, const char& ch1, const char& ch2);
};

NAME_SPACE_END()
#endif //!_NET_H
