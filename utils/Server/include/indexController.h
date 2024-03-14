#pragma once
#include "CController.h"
#include "Net.h"
#include "Reflex.h"
using namespace myUtil;
class indexController : public CController
{
public:
	indexController() {}
	~indexController() {}
	string index(const Request& req, Response* resp);
	string test(const Request& req, Response* resp);
};
