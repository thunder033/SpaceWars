#include "Renderer.h"

std::map<std::string, ISimpleShader*> Renderer::shaders;

//TODO: move this into the material class
void Renderer::createDefaultMaterial()
{
	//The color the default texture (white)
	unsigned char textureColor[] = { 255, 255, 255, 255 };

	//Create the default texture description
	D3D11_TEXTURE2D_DESC defaultTextureDesc;
	ZeroMemory(&defaultTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	defaultTextureDesc.Width = 1;
	defaultTextureDesc.Height = 1;
	defaultTextureDesc.MipLevels = 1;
	defaultTextureDesc.ArraySize = 1;
	defaultTextureDesc.Format = mRC->support.format;
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
	ID3D11Texture2D* defaultTexture = nullptr;
	mRC->mDevice->CreateTexture2D(&defaultTextureDesc, &defaultTextureInitData, &defaultTexture);
	mRC->mDevice->CreateShaderResourceView(defaultTexture, NULL, &defaultSrv);
	defaultTexture->Release();

	new Material("default", getVS(VS_MAIN), getPS(PS_MAIN), defaultSrv);
}

void Renderer::loadShader(std::wstring shaderFile, ISimpleShader** shader, const int &shaderType)
{

	//Create a shader of the appropriate type
	switch (shaderType)
	{
	case SS_VERTEX:
		*shader = new SimpleVertexShader(mRC->mDevice, mRC->mContext);
		break;
	case SS_PIXEL:
		*shader = new SimplePixelShader(mRC->mDevice, mRC->mContext);
		break;
	}
		
	//Load the shader file
	std::wstring filePath = L"Debug/" + shaderFile;
	if (!(*shader)->LoadShaderFile(filePath.c_str()))
		(*shader)->LoadShaderFile(shaderFile.c_str());

	// You'll notice that the code above attempts to load each
	// compiled shader file (.cso) from two different relative paths.

	// This is because the "working directory" (where relative paths begin)
	// will be different during the following two scenarios:
	//  - Debugging in VS: The "Project Directory" (where your .cpp files are) 
	//  - Run .exe directly: The "Output Directory" (where the .exe & .cso files are)

	// Checking both paths is the easiest way to ensure both 
	// scenarios work correctly, although others exist
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
	loadShader(L"VertexShader.cso", &shaders[VS_MAIN], SS_VERTEX);
	loadShader(L"PostProcessVS.cso", &shaders[VS_POST_PROCESS], SS_VERTEX);

	//Load Pixel Shaders
	loadShader(L"PixelShader.cso", &shaders[PS_MAIN], SS_PIXEL);
	loadShader(L"WireframeShader.cso", &shaders[PS_WIREFRAME], SS_PIXEL);
	loadShader(L"PostProcessPS.cso", &shaders[PS_POST_PROCESS], SS_PIXEL);
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

	mRC->mDevice->CreateSamplerState(&samplerDesc, &sampler);
}

Renderer::Renderer(DXResourceContext* rc)
{
	this->mRC = rc;

	//Get some common render states from DTK
	renderStates = std::make_unique<CommonStates>(mRC->mDevice);

	loadShaders();
	createSampler();
	createDefaultMaterial();

	spriteBatch = std::unique_ptr<SpriteBatch>(new SpriteBatch(mRC->mContext));
	spriteFont = std::unique_ptr<SpriteFont>(new SpriteFont(mRC->mDevice, L"Debug/Assets/Textures/font.spritefont"));

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

	HRESULT created = mRC->mDevice->CreateRasterizerState(&RSWireFrameDesc, &wireFrameState);
	
	//Create Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = mRC->support.format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	DX::ThrowIfFailed(mRC->mDevice->CreateShaderResourceView(mRC->mPostProcessRT, &srvDesc, &postProcessSRV));
}



Renderer::~Renderer()
{
	for (auto kvp : shaders) 
	{
		delete kvp.second;
	}

	if (sampler) { sampler->Release(); }
	if (defaultSrv) { defaultSrv->Release(); }


	if (postProcessSRV) { postProcessSRV->Release(); }

	if (wireFrameState) { wireFrameState->Release(); }

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
	mRC->mContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	mRC->mContext->IASetIndexBuffer(mesh->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	mRC->mContext->RSSetState(renderStates->CullCounterClockwise());
	// Finally do the actual drawing
	//  - Do this ONCE PER OBJECT you intend to draw
	//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//     vertices in the currently set VERTEX BUFFER
	mRC->mContext->DrawIndexed(
		mesh->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices

	getPS(PS_WIREFRAME)->CopyAllBufferData();
	getPS(PS_WIREFRAME)->SetShader();
	mRC->mContext->RSSetState(wireFrameState);
	//DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1);
	//renderer->getWireframeShader()->SetData("Color", &color, sizeof(DirectX::XMFLOAT4));
	mRC->mContext->DrawIndexed(
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

void Renderer::clearRenderTargets()
{
	//Clear color (black)
	const float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	mRC->mContext->ClearRenderTargetView(mRC->mPrimaryRTV, color);
	mRC->mContext->ClearRenderTargetView(mRC->mPostProcessRTV, color);
	mRC->mContext->ClearRenderTargetView(mRC->mBackBufferRTV, color);

	mRC->mContext->ClearDepthStencilView(
		mRC->mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
}

void Renderer::postProcess(UINT stride, UINT offset, ID3D11Texture2D* renderTargetMS)
{
	unsigned int sub = D3D11CalcSubresource(0, 0, 1);

	mRC->mContext->ResolveSubresource(
		mRC->mPostProcessRT,
		sub,
		renderTargetMS,
		sub,
		mRC->support.format
	);

	// Reset the states!
	mRC->mContext->RSSetState(0);
	mRC->mContext->OMSetDepthStencilState(0, 0);

	getVS(VS_POST_PROCESS)->SetShader();

	getPS(PS_POST_PROCESS)->SetShader();
	getPS(PS_POST_PROCESS)->SetShaderResourceView("Pixels", postProcessSRV);
	getPS(PS_POST_PROCESS)->SetInt("blurAmount", 3);
	getPS(PS_POST_PROCESS)->SetFloat("pixelWidth", 1.0f / mRC->dimensions.width);
	getPS(PS_POST_PROCESS)->SetFloat("pixelHeight", 1.0f / mRC->dimensions.height);
	getPS(PS_POST_PROCESS)->CopyAllBufferData();

	ID3D11Buffer* nothing = 0;
	mRC->mContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	mRC->mContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	mRC->mContext->Draw(3, 0);

	getPS(PS_POST_PROCESS)->SetShaderResourceView("Pixels", 0);
}
