#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include <map>

class Material
{
	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	ID3D11ShaderResourceView* textureSrv;
	DirectX::XMFLOAT4 color;

	static std::map<std::string, Material*> materials;

public:
	Material(std::string name, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* srv);
	Material(std::string name, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, DirectX::XMFLOAT4 color, ID3D11ShaderResourceView* srv);

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

	/*Static Material Management*/
	//static void createDefault(); TODO: imlement from renderer in this class
	static Material* getDefault();
	static Material* getByName(std::string name);
	static void release();
};

