#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include <DirectXMath.h>

using namespace DirectX;

class Renderer
{
	ID3D11Device*			device;
	ID3D11DeviceContext*	context;

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	ID3D11SamplerState* sampler;

	ID3D11Texture2D* defaultTexture;
	ID3D11ShaderResourceView* defaultSrv;
	Material* baseMaterial;

	void loadShaders();
	void createSampler();
	void createDefaultMaterial();
	
public:
	Renderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~Renderer();

	void render(Entity* entity, Camera* camera);
	
	//Send light data to the pixel shader
	void setLightData(DirectionalLight* lights);

	Material* getDefaultMaterial() {
		return baseMaterial;
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

	ID3D11ShaderResourceView* getDefaultTexture() {
		return defaultSrv;
	}
};

