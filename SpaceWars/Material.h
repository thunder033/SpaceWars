#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

class Material
{
	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	ID3D11ShaderResourceView* textureSrv;
	DirectX::XMFLOAT4 color;

public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* srv);
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, DirectX::XMFLOAT4 color, ID3D11ShaderResourceView* srv);

	virtual ~Material();

	SimpleVertexShader* getVertexShader() {
		return vertexShader;
	};

	SimplePixelShader* getPixelShader() {
		return pixelShader;
	}

	ID3D11ShaderResourceView* getTexture() {
		return textureSrv;
	}

	DirectX::XMFLOAT4 getColor() {
		return color;
	}
};

