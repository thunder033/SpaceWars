#pragma once

#include "GameObject.h"

class Octree;

class OctNode
{
public:
	OctNode(Octree* octree, OctNode* parent);
	~OctNode();
	void generateLeaves();
	bool isLeaf();
	bool hasObjects();
	OctNode* getLeaf(int whichLeaf);
	void addObjects(const std::vector<GameObject*> &objects);
	OctNode* getParent();
	const std::vector<GameObject*>& getObjects() const;
private:
	Octree* octTree;
	std::vector<GameObject*> containedObjects;
	OctNode** leaves;
	OctNode* parent;
};