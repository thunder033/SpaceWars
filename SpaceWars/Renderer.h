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
#include "ParticleEmitter.h"

using namespace DirectX;

#define SS_VERTEX 0
#define SS_PIXEL 1

#define VS_MAIN "mainVS"
#define VS_POST_PROCESS "postProcessVS"
#define VS_PARTICLE "particleVS"

#define PS_MAIN "mainPS"
#define PS_WIREFRAME "wireframePS"
#define PS_FIELD_BLUR "fieldBlurPS"
#define PS_BLOOM_EXTRACT "bloomExtractPS"
#define PS_BLOOM_COMPOSITE "bloomCompositePS"
#define PS_GAUSSION_BLUR "gaussionBlurPS"
#define PS_PARTICLE "particlePS"

class Renderer
{
	DXResourceContext* mRC;

	static std::map<std::string, ISimpleShader*> shaders;

	ID3D11SamplerState* sampler;

	ID3D11ShaderResourceView* defaultSrv;
	
	//Post processing
	std::unique_ptr<DirectX::CommonStates> renderStates;
	ID3D11RasterizerState* wireFrameState;
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;

	std::unique_ptr<SpriteBatch> spriteBatch;
	std::unique_ptr<SpriteFont> spriteFont;

	void loadShaders();
	void loadShader(std::wstring shaderFile, ISimpleShader** shader, const int &shaderType);

	void createSampler();
	void createDefaultMaterial();
	
public:
	Renderer(DXResourceContext* rc);
	~Renderer();

	//TODO: implement this
	void render(GameObject* gameObject, Camera* camera);
	void render(ParticleEmitter* emitter, Camera* camera);
	
	//Send light data to the pixel shader
	void setLightData(DirectionalLight* lights);

	//2D sprite rendering for HUD
	SpriteBatch* getSpriteBatch();
	SpriteFont* getSpriteFont();

	//Rendering Functions
	void clearRenderTargets();
	void postProcess(UINT stride, UINT offset, ID3D11Texture2D* renderTargetMS);

	ID3D11RasterizerState* getWireFrameState() const {
		return wireFrameState;
	}

	ID3D11SamplerState* getSampler() {
		return sampler;
	}

	SimpleVertexShader* getVertexShader() {
		return  static_cast<SimpleVertexShader*>(shaders[VS_MAIN]);
	}

	SimplePixelShader* getPixelShader() {
		return static_cast<SimplePixelShader*>(shaders[PS_MAIN]);
	}

	SimplePixelShader* getWireframeShader() {
		return static_cast<SimplePixelShader*>(shaders[PS_WIREFRAME]);
	}

	static SimpleVertexShader* getVS(std::string name) {
		return static_cast<SimpleVertexShader*>(shaders[name]);
	}

	static SimplePixelShader* getPS(std::string name) {
		return static_cast<SimplePixelShader*>(shaders[name]);
	}

	ID3D11ShaderResourceView* getDefaultTexture() {
		return defaultSrv;
	}

	CommonStates* getCommonStates() {
		return renderStates.get();
	}
};

