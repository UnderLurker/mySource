#pragma once
#include "indexController.h"
#define REFLEX_DECLARE \
REGISTER_REFLEX(indexController)\
REGISTER_REFLEX_METHOD_ARGS(indexController, index, string, Request&, Response*)\
REGISTER_REFLEX_METHOD_ARGS(indexController, test, string, Request&, Response*)