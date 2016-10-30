#pragma once
#include "Scene.h"
#include "DXCore.h"
#include <DirectXMath.h>

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();

	void init();
	void update(float deltaTime, float totalTime);
	void draw(float deltaTime, float totalTime, Renderer* renderer);
	void keyboardInput();
};

