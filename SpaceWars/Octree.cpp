#include "Octree.h"

Octree::Octree(Vector3 min, Vector3 max, int maxObjects)
{
	this->head = new OctNode(this, nullptr);
	this->minBoundary = min;
	this->maxBoundary = max;
	this->maxObjects = maxObjects;
}

void deleteHelper(OctNode* tempHead)
{
	if (tempHead == nullptr) return;

	if (tempHead->isLeaf())
	{
		delete tempHead;
	}
	else
	{
		deleteHelper(tempHead->getLeaf(0));
		deleteHelper(tempHead->getLeaf(1));
		deleteHelper(tempHead->getLeaf(2));
		deleteHelper(tempHead->getLeaf(3));
		deleteHelper(tempHead->getLeaf(4));
		deleteHelper(tempHead->getLeaf(5));
		deleteHelper(tempHead->getLeaf(6));
		deleteHelper(tempHead->getLeaf(7));

		delete tempHead;
	}
}

Octree::~Octree()
{
	deleteHelper(head);
	head = nullptr;
}

int Octree::getMaxObjects()
{
	return this->maxObjects;
}

void generateHelper(unsigned int maxObjects, std::vector<GameObject*> &objects, OctNode* start, Vector3 min, Vector3 max)
{
	if (objects.size() > 0)
	{
		std::vector<GameObject*> containedObjects = std::vector<GameObject*>();
		for (GameObject* temp : objects)
		{
			Vector3 tempMin = temp->GetCollider()->getMin() + temp->getPosition();
			Vector3 tempMax = temp->GetCollider()->getMax() + temp->getPosition();
			if (tempMin.x >= min.x && tempMax.x <= max.x &&
				tempMin.y >= min.y && tempMax.y <= max.y &&
				tempMin.z >= min.z && tempMax.z <= max.z)
			{
				containedObjects.push_back(temp);
			}
		}
		if (containedObjects.size() > maxObjects)
		{
			Vector3 center = ((min + max) / 2.f);
			start->generateLeaves();
			generateHelper(maxObjects, containedObjects, start->getLeaf(0), min, ((min + max) / 2.f));
			generateHelper(maxObjects, containedObjects, start->getLeaf(1), min + Vector3((max.x - min.x) / 2.f, 0.0f, 0.0f),
				center + Vector3((max.x - min.x) / 2.f, 0.0f, 0.0f));
			generateHelper(maxObjects, containedObjects, start->getLeaf(2), min + Vector3(0.f, 0.f, (max.z - min.z) / 2.f),
				center + Vector3(0.f, 0.f, (max.z - min.z) / 2.f));
			generateHelper(maxObjects, containedObjects, start->getLeaf(3), min + Vector3((max.x - min.x) / 2.f, 0.f, (max.z - min.z) / 2.f),
				center + Vector3((max.x - min.x) / 2.f, 0.0f, (max.z - min.z) / 2.f));

			generateHelper(maxObjects, containedObjects, start->getLeaf(4), center, max);
			generateHelper(maxObjects, containedObjects, start->getLeaf(5), center - Vector3((max.x - min.x) / 2.f, 0.0f, 0.0f),
				max - Vector3((max.x - min.x) / 2.f, 0.0f, 0.0f));
			generateHelper(maxObjects, containedObjects, start->getLeaf(6), center - Vector3(0.f, 0.f, (max.z - min.z) / 2.f),
				max - Vector3(0.f, 0.f, (max.z - min.z) / 2.f));
			generateHelper(maxObjects, containedObjects, start->getLeaf(7), center - Vector3((max.x - min.x) / 2.f, 0.f, (max.z - min.z) / 2.f),
				max - Vector3((max.x - min.x) / 2.f, 0.0f, (max.z - min.z) / 2.f));
		}
		if (containedObjects.size() > 0)
		{
			for (GameObject* temp : containedObjects)
			{
				if (std::find(objects.begin(), objects.end(), temp) != objects.end())
					objects.erase(std::remove(objects.begin(), objects.end(), temp), objects.end());
				temp->setOctNode(start);
			}
			start->addObjects(containedObjects);
		}
	}
}

void Octree::generateTree(std::vector<GameObject*> objects)
{
	generateHelper(this->maxObjects, objects, this->head, this->minBoundary, this->maxBoundary);
}

void childFinderTomToldMeToDoIt(std::vector<GameObject*>& results, OctNode* current)
{
	results.insert(results.end(), current->getObjects().begin(), current->getObjects().end());
	if (!current->isLeaf())
	{
		for (int i = 0; i < 8; ++i)
		{
			childFinderTomToldMeToDoIt(results, current->getLeaf(i));
		}
	}
}

std::vector<GameObject*> Octree::nearbyObjects(GameObject* object)
{

	std::vector<GameObject*> toReturn = std::vector<GameObject*>();
	OctNode* start = object->getOctNode();
	do
	{
		toReturn.insert(toReturn.end(), start->getObjects().begin(), start->getObjects().end());
		start = start->getParent();
	} while (start);

	start = object->getOctNode();
	if (!start->isLeaf())
	{
		for (int i = 0; i < 8; ++i)
		{
			childFinderTomToldMeToDoIt(toReturn, start->getLeaf(i));
		}
	}

	return toReturn;
}
