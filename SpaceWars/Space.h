#pragma once
#include "Scene.h"
class Space :
	public Scene
{
public:
	Space();
	~Space();

	void init();
	void update(float dt, float tt);
	void draw(float dt, float tt, Renderer* renderer);
};

