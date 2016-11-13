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

using namespace DirectX;

class Renderer
{
	ID3D11Device*			device;
	ID3D11DeviceContext*	context;

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimplePixelShader * wireframeShader;

	ID3D11SamplerState* sampler;

	ID3D11Texture2D* defaultTexture;
	ID3D11ShaderResourceView* defaultSrv;

	ID3D11RasterizerState* wireFrameState;

	std::unique_ptr<SpriteBatch> spriteBatch;
	std::unique_ptr<SpriteFont> spriteFont;

	void loadShaders();
	void createSampler();
	void createDefaultMaterial();
	
public:
	Renderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~Renderer();

	//TODO: implement this
	void render(GameObject* entity, Camera* camera);
	
	//Send light data to the pixel shader
	void setLightData(DirectionalLight* lights);

	//2D sprite rendering for HUD
	SpriteBatch* getSpriteBatch();
	SpriteFont* getSpriteFont();

	ID3D11RasterizerState* getWireFrameState() const {
		return wireFrameState;
	}

	ID3D11SamplerState* getSampler() {
		return sampler;
	}

	SimpleVertexShader* getVertexShader() {
		return vertexShader;
	}

	SimplePixelShader* getPixelShader() {
		return pixelShader;
	}

	SimplePixelShader* getWireframeShader() {
		return wireframeShader;
	}

	ID3D11ShaderResourceView* getDefaultTexture() {
		return defaultSrv;
	}
};

