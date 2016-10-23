#pragma once

#include <string>
#include <vector>
#include "Entity.h"

class Scene
{
	std::string name;
public:
	Scene();
	virtual ~Scene();

	virtual void init(std::vector<Entity*> &entities);
	virtual void update(float dt, float tt, std::vector<Entity*> &entities);
	virtual void draw(float dt, float tt, std::vector<Entity*> &entities);
};

