#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include "Renderer.h"
#include "Scene.h"
#include "Menu.h"
#include "Space.h"
#include <map>
#include <SpriteFont.h>
#include <SpriteBatch.h>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);

	// Game
	void StartScene(std::string sceneName);
	void ReleaseEntities();
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void CreateBasicGeometry();

	std::vector<Entity*> entities;
	Renderer* renderer;

	std::unique_ptr<SpriteBatch> spriteBatch;
	std::unique_ptr<SpriteFont> spriteFont;

	//Textures
	ID3D11ShaderResourceView* crateSrv;

	//Rendering Data
	Camera* camera;
	DirectionalLight* lights;

	//Game
	Scene* activeScene;
	std::map<std::string, Scene*> scenes;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
	bool mouseDragging;
};

