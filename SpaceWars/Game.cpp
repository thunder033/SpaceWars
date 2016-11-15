#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include <SpriteFont.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore( 
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	mouseDragging = false;
	camera = new Camera((float)width / height);

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	Scene::release();
	Mesh::release();
	Material::release();

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete lights;
	delete camera;

	crateSrv->Release();

	delete renderer;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{

	//Instantiate the renderer that stores render data and will (eventually) handle rendering
	renderer = new Renderer(device, context, this);

	//setup the camera
	camera->setAspectRatio((float)width / height);

	//Load meshes
	Mesh::loadMeshes(device);

	//Wood Texture
	CreateWICTextureFromFile(
		device, 
		context, //Providing the context will auto-generate mipmaps
		L"Debug/Assets/Textures/crate.png", 
		0, //we don't actually need the texture reference
		&crateSrv);

	//Create materials - auto indexed - maybe should be done with a factory method
	new Material("crate", renderer->getVertexShader(), renderer->getPixelShader(), crateSrv);
	new Material("blue", renderer->getVertexShader(), renderer->getPixelShader(), XMFLOAT4(0.15f, 0.15f, 1, 1), renderer->getDefaultTexture());
	new Material("red", renderer->getVertexShader(), renderer->getPixelShader(), XMFLOAT4(1, 0.15f, 0.15, 1), renderer->getDefaultTexture());
	new Material("green", renderer->getVertexShader(), renderer->getPixelShader(), XMFLOAT4(0.15f, 1, 0.15, 1), renderer->getDefaultTexture());

	CreateLights();

	//Create Scenes - auto indexed - automatically indexed, again maybe w/ factory
	new Menu();
	new Space();
	new GameScene();
	Scene::setActive("menu");

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateLights()
{
	//Create the default white-ish light
	DirectionalLight light = {};
	light.AmbientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light.DiffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 0.75f, 1.0f);
	light.Direction = DirectX::XMFLOAT3(1, -1, 0);

	//Create secondary red light
	DirectionalLight light2 = {};
	light2.AmbientColor = DirectX::XMFLOAT4(0, 0, 0, 1.0f);
	light2.DiffuseColor = DirectX::XMFLOAT4(0.3f, 0.3f, 0.18f, 1.0f);
	light2.Direction = DirectX::XMFLOAT3(0.25f, 1, 0);
	
	//store the lights in the game
	lights = new DirectionalLight[2] { light, light2 };
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->setAspectRatio((float)width / height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	camera->update(deltaTime, totalTime);

	Scene::getActive()->update(deltaTime, totalTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Black) for clearing
	const float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//Update the light data
	renderer->setLightData(lights);

	std::vector<GameObject*> entities = Scene::getActive()->getEntities();
	std::vector<GameObject*>::iterator it;
	for (it = entities.begin(); it < entities.end(); it++) {
		renderer->render(*it, camera);
	}

	Scene::getActive()->draw(deltaTime, totalTime, renderer);

	//Reset Render states
	context->OMSetBlendState(renderer->getCommonStates()->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(renderer->getCommonStates()->DepthDefault(), 0);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	if (buttonState & 0x0001) {
		mouseDragging = true;
	}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	if (!buttonState & 0x0001) {
		mouseDragging = false;
	}

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	if (mouseDragging) {
		float rotateSpeed = 0.005f;
		camera->rotate((y - prevMousePos.y) * rotateSpeed, (x - prevMousePos.x) * rotateSpeed);
	}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion