#pragma once
#include "Scene.h"
class Menu :
	public Scene
{
public:
	Menu();
	~Menu();

	virtual void init(std::vector<Entity*> &entities);
	void update(float dt, float tt, std::vector<Entity*> &entities);
	void draw(float dt, float tt, std::vector<Entity*> &entities);
};

