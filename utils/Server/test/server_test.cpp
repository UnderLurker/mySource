#include "ControllerMacro.h"
#include "InterceptorMacro.h"
#include "Server.h"
using namespace myUtil;
REFLEX_DECLARE
REFLEX_INPERCEPTOR_DECLARE

int main() {
    myUtil::Server server("127.0.0.1", 8080);
    server.setRoute("/", "indexController", "index");
    server.setRoute("/test", "indexController", "test");
    //server.setInterceptor("/test", "IndexInterceptor");
    server.listen(2);
    return 0;
}