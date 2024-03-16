#ifndef _INDEX_CONTROLLER_H
#define _INDEX_CONTROLLER_H
#include "CController.h"
#include "Net.h"
NAME_SPACE_START(myUtil)
class indexController : public CController {
public:
    indexController()           = default;
    ~indexController() override = default;
    string index(const Request& req, Response* resp);
    string test(const Request& req, Response* resp);
};
NAME_SPACE_END()
#endif //!_INDEX_CONTROLLER_H
