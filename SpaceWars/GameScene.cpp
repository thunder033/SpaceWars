#include "GameScene.h"
#include <iostream>


GameScene::GameScene() : Scene("gameScene")
{
}


GameScene::~GameScene()
{
}

void GameScene::init()
{
	entities.push_back(new GameObject(Mesh::getByIndex(0), Material::getByName("blue")));
	Scene::init();
}

void GameScene::update(float deltaTime, float totalTime)
{
	keyboardInput();
}

void GameScene::draw(float deltaTime, float totalTime, Renderer * renderer)
{
}

void GameScene::keyboardInput()
{
	if (GetAsyncKeyState('W') & 0x8000) {
		// accelerate relative to forward vector of ship
		printf("W pressed");
	}

	if (GetAsyncKeyState('S') & 0x8000) {
		// decelerate relative to forward vector of ship
		printf("S pressed");
	}

	if (GetAsyncKeyState('A') & 0x8000) {
		// rotate counter clockwise on the y-axis
		printf("A pressed");
	}

	if (GetAsyncKeyState('D') & 0x8000) {
		// rotate clockwise on the y-axis
		printf("D pressed");
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		// create a bullet entity, thinking the bullet should manage its own movement when created
		printf("Space pressed");
	}
}
