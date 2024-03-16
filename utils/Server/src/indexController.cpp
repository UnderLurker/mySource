#include "indexController.h"

#include <fstream>
using namespace std;

NAME_SPACE_START(myUtil)

string indexController::index(const Request& req, Response* resp) {
    resp->setResponseStatus("HTTP", 1, 1, 200, "OK");
    resp->setResponseHead("Content-Type", "text/html,charset=UTF-8");
    return "index.html";
}

string indexController::test(const Request& req, Response* resp) {
    resp->setResponseStatus("HTTP", 1, 1, 200, "OK");
    resp->setResponseHead("Content-Type", "text/html,charset=UTF-8");
    Cookie cookie("test", "test");
    cookie.setDomain("localhost");
    cookie.setMaxAge(10);
    cookie.setPath("/test");
    resp->setCookie(cookie);
    return "test.html";
}
NAME_SPACE_END()
