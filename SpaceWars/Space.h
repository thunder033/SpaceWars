#pragma once
#include "Scene.h"
class Space :
	public Scene
{
public:
	Space();
	~Space();

	void init();
	void update(float deltaTime, float totalTime);
	void draw(float deltaTime, float totalTime, Renderer* renderer);
};

