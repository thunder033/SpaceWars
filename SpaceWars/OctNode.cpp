#include "OctNode.h"

OctNode::OctNode(Octree* octree, OctNode* parent)
{
	this->octTree = octree;
	this->leaves = nullptr;
	this->parent = parent;
}

OctNode::~OctNode()
{
	if (leaves)
	{
		free(leaves);
	}
}

void OctNode::generateLeaves()
{
	leaves = (OctNode**)malloc(sizeof(OctNode*) * 8);
	for (int i = 0; i < 8; ++i)
	{
		leaves[i] = new OctNode(this->octTree, this);
	}
}

bool OctNode::isLeaf()
{
	return !leaves;
}

bool OctNode::hasObjects()
{
	return containedObjects.size() > 0;
}

OctNode* OctNode::getLeaf(int whichLeaf)
{
	if (whichLeaf < 0 || whichLeaf > 7) return nullptr;
	if (leaves != nullptr) return leaves[whichLeaf];
	return nullptr;
}

void OctNode::addObjects(const std::vector<GameObject*> &objects)
{
	this->containedObjects = objects;
}

OctNode * OctNode::getParent()
{
	return this->parent;
}

const std::vector<GameObject*>& OctNode::getObjects() const
{
	return this->containedObjects;
}
