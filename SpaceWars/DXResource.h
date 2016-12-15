#pragma once

#include "DXResourceContext.h"

class DXResource
{
protected:
	//Static context for accessing directX resources
	static DXResourceContext* mRC;
public:
	DXResource();
	~DXResource();

	static void setResourceContext(DXResourceContext* rc);
};

