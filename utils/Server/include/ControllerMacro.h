#ifndef _CONTROLLER_MACRO_H
#define _CONTROLLER_MACRO_H
#include "Util.h"
#include "indexController.h"

NAME_SPACE_START(myUtil)
#define REFLEX_DECLARE                                                               \
    REGISTER_REFLEX(indexController)                                                 \
    REGISTER_REFLEX_METHOD_ARGS(indexController, index, string, Request&, Response*) \
    REGISTER_REFLEX_METHOD_ARGS(indexController, test, string, Request&, Response*)
NAME_SPACE_END()
#endif //!_CONTROLLER_MACRO_H
