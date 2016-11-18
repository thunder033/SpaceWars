#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include <DirectXMath.h>
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <CommonStates.h>
#include <map>

using namespace DirectX;

#define VS_MAIN "mainVS"
#define VS_POST_PROCESS "postProcessVS"

#define PS_MAIN "mainPS"
#define PS_WIREFRAME "wireframePS"
#define PS_POST_PROCESS "postProcessPS"

class Renderer
{
	ID3D11Device*			device;
	ID3D11DeviceContext*	context;
	DXCore*					dxcore;

	static std::map<std::string, SimpleVertexShader*> vertexShaders;
	static std::map<std::string, SimplePixelShader*> pixelShaders;

	ID3D11SamplerState* sampler;

	ID3D11Texture2D* defaultTexture;
	ID3D11ShaderResourceView* defaultSrv;
	
	//Post processing
	ID3D11Texture2D* ppTexture;
	ID3D11RenderTargetView* postProcessRTV;
	ID3D11ShaderResourceView* postProcessSRV;

	std::unique_ptr<DirectX::CommonStates> renderStates;
	ID3D11RasterizerState* wireFrameState;

	std::unique_ptr<SpriteBatch> spriteBatch;
	std::unique_ptr<SpriteFont> spriteFont;

	void loadShaders();
	void createSampler();
	void createDefaultMaterial();
	
public:
	Renderer(ID3D11Device* device, ID3D11DeviceContext* context, DXCore* dxcore);
	~Renderer();

	//TODO: implement this
	void render(GameObject* gameObject, Camera* camera);
	
	//Send light data to the pixel shader
	void setLightData(DirectionalLight* lights);

	//2D sprite rendering for HUD
	SpriteBatch* getSpriteBatch();
	SpriteFont* getSpriteFont();

	//Rendering Functions
	void resetPostProcess(ID3D11DepthStencilView* depthStencilView);
	void postProcess(UINT stride, UINT offset, ID3D11Texture2D* renderTargetMS);

	ID3D11RasterizerState* getWireFrameState() const {
		return wireFrameState;
	}

	ID3D11SamplerState* getSampler() {
		return sampler;
	}

	SimpleVertexShader* getVertexShader() {
		return vertexShaders[VS_MAIN];
	}

	SimplePixelShader* getPixelShader() {
		return pixelShaders[PS_MAIN];
	}

	SimplePixelShader* getWireframeShader() {
		return pixelShaders[PS_WIREFRAME];
	}

	static SimpleVertexShader* getVS(std::string name) {
		return vertexShaders[name];
	}

	static SimplePixelShader* getPS(std::string name) {
		return pixelShaders[name];
	}

	ID3D11ShaderResourceView* getDefaultTexture() {
		return defaultSrv;
	}

	CommonStates* getCommonStates() {
		return renderStates.get();
	}
};

