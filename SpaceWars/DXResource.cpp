#include "DXResource.h"

//Static Members
DXResourceContext* DXResource::mRC = nullptr;

void DXResource::setResourceContext(DXResourceContext* rc)
{
	DXResource::mRC = rc;
}

//Instance Members
DXResource::DXResource()
{
}


DXResource::~DXResource()
{
}