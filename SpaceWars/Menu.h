#pragma once
#include "Scene.h"
class Menu :
	public Scene
{
public:
	Menu();
	~Menu();

	void init();
	void update(float deltaTime, float totalTime);
	void draw(float deltaTime, float totalTime, Renderer* renderer);
};

