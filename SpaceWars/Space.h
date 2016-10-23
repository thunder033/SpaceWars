#pragma once
#include "Scene.h"
class Space :
	public Scene
{
public:
	Space();
	~Space();

	void init(std::vector<Entity*> &entities);
	void update(float dt, float tt, std::vector<Entity*> &entities);
	void draw(float dt, float tt, std::vector<Entity*> &entities);
};

