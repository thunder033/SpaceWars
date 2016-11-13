#include "Renderer.h"


//TODO: move this into the material class
void Renderer::createDefaultMaterial()
{
	unsigned char textureColor[] = { 255, 255, 255, 255 };

	//Create the default texture description
	D3D11_TEXTURE2D_DESC defaultTextureDesc;
	ZeroMemory(&defaultTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	defaultTextureDesc.Width = 1;
	defaultTextureDesc.Height = 1;
	defaultTextureDesc.MipLevels = 1;
	defaultTextureDesc.ArraySize = 1;
	defaultTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	defaultTextureDesc.SampleDesc.Count = 1;
	defaultTextureDesc.SampleDesc.Quality = 0;
	defaultTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	defaultTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	defaultTextureDesc.CPUAccessFlags = 0;
	defaultTextureDesc.MiscFlags = 0;

	//Set the initial data of a white color
	D3D11_SUBRESOURCE_DATA defaultTextureInitData;
	ZeroMemory(&defaultTextureInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	defaultTextureInitData.pSysMem = textureColor;
	defaultTextureInitData.SysMemPitch = sizeof(unsigned char) * 4;

	//Create the Texture and SRV
	defaultTexture = nullptr;
	device->CreateTexture2D(&defaultTextureDesc, &defaultTextureInitData, &defaultTexture);
	device->CreateShaderResourceView(defaultTexture, NULL, &defaultSrv);

	new Material("default", vertexShader, pixelShader, defaultSrv);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Renderer::loadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	if (!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))
		pixelShader->LoadShaderFile(L"PixelShader.cso");

	wireframeShader = new SimplePixelShader(device, context);
	if (!wireframeShader->LoadShaderFile(L"Debug/WireframeShader.cso"))
		wireframeShader->LoadShaderFile(L"WireframeShader.cso");

	// You'll notice that the code above attempts to load each
	// compiled shader file (.cso) from two different relative paths.

	// This is because the "working directory" (where relative paths begin)
	// will be different during the following two scenarios:
	//  - Debugging in VS: The "Project Directory" (where your .cpp files are) 
	//  - Run .exe directly: The "Output Directory" (where the .exe & .cso files are)

	// Checking both paths is the easiest way to ensure both 
	// scenarios work correctly, although others exist
}

void Renderer::createSampler()
{
	//Create a sampler state for texture sampling
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);
}

Renderer::Renderer(ID3D11Device* device, ID3D11DeviceContext* context)
{
	//Get some common render states from DTK
	renderStates = std::make_unique<CommonStates>(device);

	vertexShader = 0;
	pixelShader = 0;

	this->device = device;
	this->context = context;

	loadShaders();
	createSampler();
	createDefaultMaterial();

	spriteBatch = std::unique_ptr<SpriteBatch>(new SpriteBatch(context));
	spriteFont = std::unique_ptr<SpriteFont>(new SpriteFont(device, L"Debug/Assets/Textures/font.spritefont"));

	D3D11_RASTERIZER_DESC RSWireFrameDesc;
	RSWireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	RSWireFrameDesc.CullMode = D3D11_CULL_FRONT;
	RSWireFrameDesc.FrontCounterClockwise = true;
	RSWireFrameDesc.DepthBias = false;
	RSWireFrameDesc.DepthBiasClamp = 0;
	RSWireFrameDesc.SlopeScaledDepthBias = 0;
	RSWireFrameDesc.DepthClipEnable = true;
	RSWireFrameDesc.ScissorEnable = false;
	RSWireFrameDesc.MultisampleEnable = false;
	RSWireFrameDesc.AntialiasedLineEnable = false;

	HRESULT created = this->device->CreateRasterizerState(&RSWireFrameDesc, &wireFrameState);
	// Assumes that "pDevice" is valid (ID3D11Device*) 
}



Renderer::~Renderer()
{
	delete vertexShader;
	delete pixelShader;

	sampler->Release();
	defaultSrv->Release();
	defaultTexture->Release();
}

void Renderer::render(GameObject * gameObject, Camera * camera)
{
	gameObject->getMaterial()->prepare(camera->getViewMatrix(), camera->getProjectionMatrix(), gameObject->getDrawMatrix(), sampler);

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	Mesh* mesh = gameObject->getMesh();
	ID3D11Buffer* vertexBuffer = mesh->getVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	context->RSSetState(renderStates->CullCounterClockwise());
	// Finally do the actual drawing
	//  - Do this ONCE PER OBJECT you intend to draw
	//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//     vertices in the currently set VERTEX BUFFER
	context->DrawIndexed(
		mesh->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices

	wireframeShader->CopyAllBufferData();
	wireframeShader->SetShader();
	context->RSSetState(wireFrameState);
	//DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1);
	//renderer->getWireframeShader()->SetData("Color", &color, sizeof(DirectX::XMFLOAT4));
	context->DrawIndexed(
		mesh->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}

void Renderer::setLightData(DirectionalLight * lights)
{
	//This is currently hard coded for 2 lights
	pixelShader->SetData("light", &lights[0], sizeof(DirectionalLight));
	pixelShader->SetData("light2", &lights[1], sizeof(DirectionalLight));
}

SpriteBatch* Renderer::getSpriteBatch()
{
	return spriteBatch.get();
}

SpriteFont * Renderer::getSpriteFont()
{
	return spriteFont.get();
}
