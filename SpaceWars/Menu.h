#pragma once
#include "Scene.h"
class Menu :
	public Scene
{
public:
	Menu();
	~Menu();

	void init();
	void update(float dt, float tt);
	void draw(float dt, float tt, Renderer* renderer);
};

