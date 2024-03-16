#include "Net.h"
NAME_SPACE_START(myUtil)

Request::Request(const string& sourceStr) {
    int i                   = 0;
    vector<string> reqGroup = Analyse::getVectorBySplit(sourceStr, '\n');
    // 解析状态行
    vector<string> statuses = Analyse::getVectorBySplit(reqGroup[0], ' ');
    _status.RMethod         = statuses.at(0);
    _status.Url             = statuses.at(1);
    statuses.at(2).pop_back();
    vector<string> verInfo = Analyse::getVectorBySplit(statuses.at(2), '/');
    _status.ProName        = verInfo.at(0);
    _status.verHigh        = (short)(Analyse::getVectorBySplit(verInfo.at(1), '.').at(0).at(0) - '0');
    _status.verLow         = (short)(Analyse::getVectorBySplit(verInfo.at(1), '.').at(1).at(0) - '0');
    // 解析请求头
    for (i = 1; i < reqGroup.size(); i++) {
        if (reqGroup[i] == "\r") break;
        reqGroup[i].pop_back();
        vector<string> temp = Analyse::getVectorBySplit(reqGroup[i], ':');
        _RequestHead.insert(pair<string, string>(temp.at(0), temp.at(1)));
    }
    i++;
    for (i; i < reqGroup.size(); i++) {
        _RequestContent += reqGroup.at(i) + "\n";
    }
}

void Request::setRequestStatus(const string& method,
                               const string& url,
                               const string& _proName,
                               const short& _verHigh,
                               const short& _verLow) {
    _status.RMethod = method;
    _status.Url     = url;
    _status.ProName = _proName;
    _status.verHigh = _verHigh;
    _status.verLow  = _verLow;
}

void Request::setRequestHead(const string& headKey, const string& headValue) {
    _RequestHead.insert(pair<string, string>(headKey, headValue));
}

void Request::setRequestContent(const string& content) { _RequestContent = content; }

RequestStatus Request::getRequestStatus() const { return _status; }

string Request::getRequestContent(const string& headKey) const { return _RequestContent; }

string Request::toString() {
    string res;
    res += _status.RMethod + SPACE + _status.Url + SPACE + _status.ProName + "/" + to_string(_status.verHigh) + "." +
           to_string(_status.verLow) + BLACK;
    for (auto& it : _RequestHead) {
        res += it.first + ":" + SPACE + it.second + BLACK;
    }
    res += BLACK;
    res += _RequestContent;
    return res;
}

vector<Cookie> Request::getCookie() const { return _cookie; }

void Request::setCookie(const Cookie& cookie) { _cookie.push_back(cookie); }

Response::Response() {
    _status.ProName    = "HTTP";
    _status.verHigh    = 1;
    _status.verLow     = 1;
    _status.status     = 200;
    _status.statusWord = "OK";
}

void Response::setResponseStatus(const string& _proName,
                                 const short& _verHigh,
                                 const short& _verLow,
                                 const short& status,
                                 const string& word) {
    _status.ProName    = _proName;
    _status.verHigh    = _verHigh;
    _status.verLow     = _verLow;
    _status.status     = status;
    _status.statusWord = word;
}

void Response::setResponseHead(const string& headKey, const string& headValue) {
    _ResponseHead.insert(pair<string, string>(headKey, headValue));
}

void Response::setResponseContent(const string& content) { _ResponseContent = content; }

ResponseStatus Response::getResponseStatus() const { return _status; }

string Response::getResponseHeadByKey(const string& headKey) const {
    auto it = _ResponseHead.find(headKey);
    if (it == _ResponseHead.end()) return "";
    return (*it).second;
}

vector<Cookie> Response::getCookie() const { return _cookie; }

void Response::setCookie(const Cookie& cookie) { _cookie.push_back(cookie); }

string Response::toString() {
    string res;
    res        += _status.ProName + "/" + to_string(_status.verHigh) + "." + to_string(_status.verLow) + SPACE +
           to_string(_status.status) + SPACE + _status.statusWord + BLACK;
    for (auto & it : _ResponseHead) {
        res += it.first + ":" + SPACE + it.second + BLACK;
    }
    res += BLACK;
    res += _ResponseContent;
    return res;
}

vector<string> Analyse::getVectorBySplit(const string& source, const char& ch) {
    vector<string> res;
    string temp;
    char t;
    for (char i : source) {
        if (i == ch) {
            res.push_back(temp);
            temp = "";
        } else {
            t = i;
            temp.push_back(ch);
        }
    }
    if (!temp.empty()) res.push_back(temp);
    return res;
}

unordered_map<string, string> Analyse::getMapBySplit(const string& source, const char& ch1, const char& ch2) {
    unordered_map<string, string> res;
    vector<string> temp = getVectorBySplit(source, ch1);
    for (const string& str : temp) {
        vector<string> t = getVectorBySplit(str, ch2);
        if (t.size() != 2) continue;
        res.insert(pair<string, string>(t.at(0), t.at(1)));
    }
    return res;
}

string Analyse::makeVectorByChar(const vector<string>& v, const char& ch) {
    string res;
    for (const auto& str : v) {
        res += str + ch;
    }
    res.pop_back();
    return res;
}

string Analyse::makeMapByChars(const unordered_map<string, string>& m, const char& ch1, const char& ch2) {
    string res;
    for (const auto& it : m) {
        res += it.first + ch2 + it.second + ch1;
    }
    res.pop_back();
    return res;
}
NAME_SPACE_END()
