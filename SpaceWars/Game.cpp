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
	std::vector<Entity*>::iterator it;
	for (it = entities.begin(); it < entities.end(); it++) {
		delete *it;
	}

	if (meshes != nullptr) {
		for (int i = 0; i < MESH_COUNT; i++) {
			if (meshes[i] != nullptr) {
				delete meshes[i];
			}
		}
		delete meshes;
	}

	//Clean up materials
	for (auto kv : materials) {
		delete kv.second;
	}

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
	renderer = new Renderer(device, context);

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	camera->setAspectRatio((float)width / height);

	//Wood Texture
	CreateWICTextureFromFile(
		device, 
		context, //Providing the context will auto-generate mipmaps
		L"Debug/Assets/Textures/crate.png", 
		0, //we don't actually need the texture reference
		&crateSrv);

	materials["crate"] = new Material(renderer->getVertexShader(), renderer->getPixelShader(), crateSrv);
	materials["blue"] =  new Material(renderer->getVertexShader(), renderer->getPixelShader(), XMFLOAT4(0.15f, 0.15f, 1, 1), renderer->getDefaultTexture());

	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	//Load meshes
	meshes = new Mesh*[MESH_COUNT]{
		new Mesh("Debug/Assets/Models/cone.obj", device),
		new Mesh("Debug/Assets/Models/cube.obj", device),
		new Mesh("Debug/Assets/Models/cylinder.obj", device),
		new Mesh("Debug/Assets/Models/helix.obj", device),
		new Mesh("Debug/Assets/Models/sphere.obj", device),
		new Mesh("Debug/Assets/Models/torus.obj", device),
	};

	//Create the default white-ish light
	DirectionalLight light = {};
	light.AmbientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light.DiffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 0.75f, 1.0f);
	light.Direction = DirectX::XMFLOAT3(1, -1, 0);


	//Create secondary red light
	DirectionalLight light2 = {};
	light2.AmbientColor = DirectX::XMFLOAT4(0, 0, 0, 1.0f);
	light2.DiffuseColor = DirectX::XMFLOAT4(1.0f, 0, 0, 1.0f);
	light2.Direction = DirectX::XMFLOAT3(-1, 1, 0);
	
	//store the lights in the game
	lights = new DirectionalLight[2] { light, light2 };

	//Create entities to be rendered
	Material* baseMaterial = renderer->getDefaultMaterial();
	entities.push_back(new Entity(meshes[0], baseMaterial));
	entities.push_back(new Entity(meshes[1], materials["crate"]));
	entities.push_back(new Entity(meshes[2], baseMaterial));
	entities.push_back(new Entity(meshes[3], materials["blue"]));
	entities.push_back(new Entity(meshes[4], baseMaterial));
	entities.push_back(new Entity(meshes[5], materials["blue"]));

	//move the first mesh to an arbitrary position
	entities[0]->getTransform()->SetPosition(1.5f, 0, 0);

	spriteBatch = std::unique_ptr<SpriteBatch>(new SpriteBatch(context));
	spriteFont = std::unique_ptr<SpriteFont>(new SpriteFont(device, L"Debug/Assets/Textures/font.spritefont"));
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

	camera->Update(deltaTime, totalTime);

	//Make the cone orbit
	entities[0]->getTransform()->SetPosition(cos(totalTime), sin(totalTime), 0);
	entities[0]->getTransform()->SetRotation(0, 0, totalTime);

	//Spinning Cube
	entities[1]->getTransform()->SetPosition(0, sin(totalTime), -2);
	entities[1]->getTransform()->SetRotation(sin(totalTime) * 3.14f, sin(totalTime) * 3.14f, sin(totalTime) * 3.14f);

	//Move stuff out of the way
	entities[2]->getTransform()->SetPosition(-2, 0, 0);
	entities[3]->getTransform()->SetPosition(-4, 0, 0);
	entities[4]->getTransform()->SetPosition(4, 0, 0); //sphere

	//Make the Torus spin
	entities[5]->getTransform()->SetPosition(4, 0, 0);
	entities[5]->getTransform()->SetRotation(totalTime * 3, 0, totalTime * 3);
	entities[5]->getTransform()->SetScale(2, 2, 2);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = {0.4f, 0.6f, 0.75f, 0.0f};

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

	std::vector<Entity*>::iterator it;
	for (it = entities.begin(); it < entities.end(); it++) {
		(*it)->prepareMaterial(camera->getViewMatrix(), camera->getProjectionMatrix(), renderer->getSampler());

		// Set buffers in the input assembler
		//  - Do this ONCE PER OBJECT you're drawing, since each object might
		//    have different geometry.
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		Mesh* mesh = (*it)->getMesh();
		ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		// Finally do the actual drawing
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		context->DrawIndexed(
			mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices

	}

	//Vector3 cameraRot = camera->getTransform()->GetRotation();

	//spriteBatch->Begin();
	//std::wstring rot(L"(" + std::to_wstring(cameraRot.x) + L", " + std::to_wstring(cameraRot.y) + L", " + std::to_wstring(cameraRot.z) + L")");
	//const wchar_t* text = rot.c_str();
	//spriteFont->DrawString(spriteBatch.get(), text, XMFLOAT2(10, 10));
	//spriteBatch->End();

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