#include "Renderer.h"

std::map<std::string, SimpleVertexShader*> Renderer::vertexShaders;
std::map<std::string, SimplePixelShader*> Renderer::pixelShaders;

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

	new Material("default", vertexShaders[VS_MAIN], pixelShaders[PS_MAIN], defaultSrv);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Renderer::loadShaders()
{
	//Load Vertext Shaders
	vertexShaders[VS_MAIN] = new SimpleVertexShader(device, context);
	if (!getVS(VS_MAIN)->LoadShaderFile(L"Debug/VertexShader.cso"))
		getVS(VS_MAIN)->LoadShaderFile(L"VertexShader.cso");

	vertexShaders[VS_POST_PROCESS] = new SimpleVertexShader(device, context);
	if (!getVS(VS_POST_PROCESS)->LoadShaderFile(L"Debug/PostProcessVS.cso"))
		getVS(VS_POST_PROCESS)->LoadShaderFile(L"PostProcessVS.cso");

	//Load Pixel Shaders
	pixelShaders[PS_MAIN] = new SimplePixelShader(device, context);
	if (!getPS(PS_MAIN)->LoadShaderFile(L"Debug/PixelShader.cso"))
		getPS(PS_MAIN)->LoadShaderFile(L"PixelShader.cso");

	pixelShaders[PS_WIREFRAME] = new SimplePixelShader(device, context);
	if (!getPS(PS_WIREFRAME)->LoadShaderFile(L"Debug/WireframeShader.cso"))
		getPS(PS_WIREFRAME)->LoadShaderFile(L"WireframeShader.cso");

	pixelShaders[PS_POST_PROCESS] = new SimplePixelShader(device, context);
	if (!getPS(PS_POST_PROCESS)->LoadShaderFile(L"Debug/PostProcessPS.cso"))
		getPS(PS_POST_PROCESS)->LoadShaderFile(L"PostProcessPS.cso");

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

Renderer::Renderer(ID3D11Device* device, ID3D11DeviceContext* context, DXCore* dxcore)
{
	//Get some common render states from DTK
	renderStates = std::make_unique<CommonStates>(device);

	this->device = device;
	this->context = context;
	this->dxcore = dxcore;

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
	RSWireFrameDesc.MultisampleEnable = true;
	RSWireFrameDesc.AntialiasedLineEnable = true;

	HRESULT created = this->device->CreateRasterizerState(&RSWireFrameDesc, &wireFrameState);
	// Assumes that "pDevice" is valid (ID3D11Device*) 

	//Create post process stuff
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = dxcore->getWidth();
	textureDesc.Height = dxcore->getHeight();
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* ppTexture;
	device->CreateTexture2D(&textureDesc, 0, &ppTexture);

	//Create Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(ppTexture, &rtvDesc, &postProcessRTV);

	//Create Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	device->CreateShaderResourceView(ppTexture, &srvDesc, &postProcessSRV);

	ppTexture->Release();
}



Renderer::~Renderer()
{
	for (auto kvp : pixelShaders) 
	{
		delete kvp.second;
	}

	for (auto kvp : vertexShaders)
	{
		delete kvp.second;
	}

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

	getPS(PS_WIREFRAME)->CopyAllBufferData();
	getPS(PS_WIREFRAME)->SetShader();
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
	getPS(PS_MAIN)->SetData("light", &lights[0], sizeof(DirectionalLight));
	getPS(PS_MAIN)->SetData("light2", &lights[1], sizeof(DirectionalLight));
}

SpriteBatch* Renderer::getSpriteBatch()
{
	return spriteBatch.get();
}

SpriteFont * Renderer::getSpriteFont()
{
	return spriteFont.get();
}

void Renderer::resetPostProcess(ID3D11DepthStencilView* depthStencilView)
{
	const float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	context->ClearRenderTargetView(postProcessRTV, color);
	context->OMSetRenderTargets(1, &postProcessRTV, depthStencilView);
}

void Renderer::postProcess(UINT stride, UINT offset)
{
	// Reset the states!
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);

	getVS(VS_POST_PROCESS)->SetShader();

	getPS(PS_POST_PROCESS)->SetShader();
	getPS(PS_POST_PROCESS)->SetShaderResourceView("Pixels", postProcessSRV);
	getPS(PS_POST_PROCESS)->SetInt("blurAmount", 3);
	getPS(PS_POST_PROCESS)->SetFloat("pixelWidth", 1.0f / dxcore->getWidth());
	getPS(PS_POST_PROCESS)->SetFloat("pixelHeight", 1.0f / dxcore->getHeight());
	getPS(PS_POST_PROCESS)->CopyAllBufferData();

	ID3D11Buffer* nothing = 0;
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	context->Draw(3, 0);

	getPS(PS_POST_PROCESS)->SetShaderResourceView("Pixels", 0);
}
