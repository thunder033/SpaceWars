#pragma once

#include "GameObject.h"
#include "OctNode.h"
#include <cstdint>

class Octree
{
public:
	Octree(Vector3, Vector3, int);
	~Octree();
	
	int getMaxObjects();
	void generateTree(std::vector<GameObject*> objects);
	std::vector<GameObject*> nearbyObjects(GameObject* object);
	bool enabled = true;
private:
	OctNode* head;
	Vector3 minBoundary;
	Vector3 maxBoundary;
	unsigned int maxObjects;
};