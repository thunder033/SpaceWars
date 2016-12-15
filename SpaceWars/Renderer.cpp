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
	loadShader(L"ParticleVS.cso", &shaders[VS_PARTICLE], SS_VERTEX);

	//Load Pixel Shaders
	loadShader(L"PixelShader.cso", &shaders[PS_MAIN], SS_PIXEL);
	loadShader(L"WireframeShader.cso", &shaders[PS_WIREFRAME], SS_PIXEL);
	loadShader(L"FieldBlur.cso", &shaders[PS_FIELD_BLUR], SS_PIXEL);
	loadShader(L"GaussionBlur.cso", &shaders[PS_GAUSSION_BLUR], SS_PIXEL);
	loadShader(L"BloomExtract.cso", &shaders[PS_BLOOM_EXTRACT], SS_PIXEL);
	loadShader(L"BloomComposite.cso", &shaders[PS_BLOOM_COMPOSITE], SS_PIXEL);
	loadShader(L"ParticlePS.cso", &shaders[PS_PARTICLE], SS_PIXEL);
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

	// A depth state for the particles
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mRC->mDevice->CreateDepthStencilState(&dsDesc, &particleDepthState);


	// Blend for particles (additive)
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	mRC->mDevice->CreateBlendState(&blend, &particleBlendState);
}



Renderer::~Renderer()
{
	for (auto kvp : shaders) 
	{
		delete kvp.second;
	}

	if (sampler) { sampler->Release(); }
	if (defaultSrv) { defaultSrv->Release(); }

	if (wireFrameState) { wireFrameState->Release(); }

}

void Renderer::render(ParticleEmitter* emitter, Camera* camera)
{
	auto headPos = emitter->getHeadPosition();
	auto tailPos = emitter->getTailPosition();
	int& particleCount = emitter->getMaxParticleCount();

	if (headPos == tailPos && emitter->canEmit())
	{
		return;
	}

	// Particle states
	float blend[4] = { 1,1,1,1 };
	mRC->mContext->OMSetBlendState(particleBlendState, blend, 0xffffffff);  // Additive blending
	mRC->mContext->OMSetDepthStencilState(particleDepthState, 0);			// No depth WRITING

	emitter->copyBuffers(mRC->mContext);

	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = emitter->getVertexBuffer();
	mRC->mContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	mRC->mContext->IASetIndexBuffer(emitter->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	mRC->mContext->RSSetState(wireFrameState);

	getVS(VS_PARTICLE)->SetMatrix4x4("view", camera->getViewMatrix());
	getVS(VS_PARTICLE)->SetMatrix4x4("projection", camera->getProjectionMatrix());
	getVS(VS_PARTICLE)->SetShader();
	getVS(VS_PARTICLE)->CopyAllBufferData();

	getPS(PS_PARTICLE)->SetSamplerState("trilinear", sampler);
	getPS(PS_PARTICLE)->SetShaderResourceView("particle", emitter->getMaterial()->getTexture());
	getPS(PS_PARTICLE)->SetShader();
	getPS(PS_PARTICLE)->CopyAllBufferData();

	if (headPos > tailPos)
	{
		mRC->mContext->DrawIndexed((headPos - tailPos) * 6, tailPos * 6, 0);
	}
	else {
		mRC->mContext->DrawIndexed(headPos * 6, 0, 0);
		mRC->mContext->DrawIndexed((particleCount - tailPos) * 6, tailPos * 6, 0);
	}

	// Reset to default states for next frame
	mRC->mContext->OMSetBlendState(0, blend, 0xffffffff);
	mRC->mContext->OMSetDepthStencilState(0, 0);
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

	getPS(PS_WIREFRAME)->SetShader();
	getPS(PS_WIREFRAME)->CopyAllBufferData();
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

	//Setup context for post processing
	ID3D11Buffer* nothing = 0;
	mRC->mContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	mRC->mContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	ID3D11ShaderResourceView* null[] = { nullptr, nullptr };

	getVS(VS_POST_PROCESS)->SetShader();

	// POST PROCESS SHADERS
	//--------------------------------------------------------------------------------
	// 1. Extract a bloom map from the scene
	//--------------------------------------------------------------------------------
	mRC->mContext->PSSetShaderResources(0, 2, null);
	mRC->mContext->OMSetRenderTargets(1, &mRC->mBloomMapRTV, 0);

	getPS(PS_BLOOM_EXTRACT)->SetShader();
	getPS(PS_BLOOM_EXTRACT)->SetShaderResourceView("ColorTexture", mRC->mPostProcessSRV);
	getPS(PS_BLOOM_EXTRACT)->SetFloat("BloomThreshold", 0.3f);
	getPS(PS_BLOOM_EXTRACT)->SetSamplerState("Sampler", sampler);
	getPS(PS_BLOOM_EXTRACT)->CopyAllBufferData();

	mRC->mContext->Draw(3, 0);

	getPS(PS_BLOOM_EXTRACT)->SetShaderResourceView("ColorTexture", 0);

	mRC->mContext->PSSetShaderResources(0, 2, null);
	mRC->mContext->OMSetRenderTargets(1, &mRC->mTemporaryRTV, 0);

	//--------------------------------------------------------------------------------
	// 2. Apply Blur to the bloom Map
	//--------------------------------------------------------------------------------
	
	/* // Field Bluffer
	getPS(PS_FIELD_BLUR)->SetShader();
	getPS(PS_FIELD_BLUR)->SetShaderResourceView("Pixels", mRC->mBloomMapSRV);
	getPS(PS_FIELD_BLUR)->SetInt("blurAmount", 3);
	getPS(PS_FIELD_BLUR)->SetFloat("pixelWidth", 1.0f / mRC->dimensions.width);
	getPS(PS_FIELD_BLUR)->SetFloat("pixelHeight", 1.0f / mRC->dimensions.height);
	getPS(PS_FIELD_BLUR)->CopyAllBufferData();

	mRC->mContext->Draw(3, 0);

	//getPS(PS_FIELD_BLUR)->SetShaderResourceView("Pixels", 0); */

	// 2.1 Gaussion Horizontal Pass
	getPS(PS_GAUSSION_BLUR)->SetShader();
	getPS(PS_GAUSSION_BLUR)->SetShaderResourceView("Pixels", mRC->mBloomMapSRV);
	getPS(PS_GAUSSION_BLUR)->SetFloat2("dir", Vector2(1.0, 0));
	getPS(PS_GAUSSION_BLUR)->SetFloat("pixelWidth", 1.0f / mRC->dimensions.width);
	getPS(PS_GAUSSION_BLUR)->SetFloat("pixelHeight", 1.0f / mRC->dimensions.height);
	getPS(PS_GAUSSION_BLUR)->CopyAllBufferData();

	mRC->mContext->Draw(3, 0);

	getPS(PS_GAUSSION_BLUR)->SetShaderResourceView("Pixels", 0);

	// 2.2 Gaussion Vertical Pass
	mRC->mContext->OMSetRenderTargets(1, &mRC->mBloomMapRTV, 0);

	getPS(PS_GAUSSION_BLUR)->SetShader();
	getPS(PS_GAUSSION_BLUR)->SetShaderResourceView("Pixels", mRC->mTemporarySRV);
	getPS(PS_GAUSSION_BLUR)->SetFloat2("dir", Vector2(0, 1.0));
	getPS(PS_GAUSSION_BLUR)->SetFloat("pixelWidth", 1.0f / mRC->dimensions.width);
	getPS(PS_GAUSSION_BLUR)->SetFloat("pixelHeight", 1.0f / mRC->dimensions.height);
	getPS(PS_GAUSSION_BLUR)->CopyAllBufferData();

	mRC->mContext->Draw(3, 0);

	//getPS(PS_FIELD_BLUR)->SetShaderResourceView("Pixels", 0);
	getPS(PS_GAUSSION_BLUR)->SetShaderResourceView("Pixels", 0);

	//--------------------------------------------------------------------------------
	// 3. Render the composite into the back buffer
	//--------------------------------------------------------------------------------
	mRC->mContext->OMSetRenderTargets(1, &mRC->mBackBufferRTV, 0);

	getPS(PS_BLOOM_COMPOSITE)->SetShader();
	getPS(PS_BLOOM_COMPOSITE)->SetShaderResourceView("ColorTexture", mRC->mPostProcessSRV);
	getPS(PS_BLOOM_COMPOSITE)->SetShaderResourceView("BloomTexture", mRC->mBloomMapSRV);
	getPS(PS_BLOOM_COMPOSITE)->SetFloat("BloomIntensity", 2.0f);
	getPS(PS_BLOOM_COMPOSITE)->SetFloat("BloomSaturation", 1.0f);
	getPS(PS_BLOOM_COMPOSITE)->SetFloat("SceneIntensity", 1.0f);
	getPS(PS_BLOOM_COMPOSITE)->SetFloat("SceneSaturation", 1.0f);
	getPS(PS_BLOOM_COMPOSITE)->SetSamplerState("Sampler", sampler);
	getPS(PS_BLOOM_COMPOSITE)->CopyAllBufferData();

	mRC->mContext->Draw(3, 0);

	//Reset resource views
	getPS(PS_BLOOM_COMPOSITE)->SetShaderResourceView("ColorTexture", 0);
	getPS(PS_BLOOM_COMPOSITE)->SetShaderResourceView("BloomTexture", 0);
}
