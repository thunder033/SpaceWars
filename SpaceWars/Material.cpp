#include "Material.h"

std::map<std::string, Material*> Material::materials;

Material::Material(std::string name, SimpleVertexShader * vertexShader, SimplePixelShader * pixelShader, ID3D11ShaderResourceView* srv)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;

	this->color = DirectX::XMFLOAT4(1, 1, 1, 1);
	this->textureSrv = srv;

	//Store this material
	Material::materials[name] = this;
}

Material::Material(std::string name, SimpleVertexShader * vertexShader, SimplePixelShader * pixelShader, DirectX::XMFLOAT4 color, ID3D11ShaderResourceView* srv)
	: Material(name, vertexShader, pixelShader, srv)
{
	this->color = color;
}


Material::~Material()
{
	
}

Material* Material::getByName(std::string name)
{
	return Material::materials[name];
}

Material* Material::getDefault()
{
	return Material::materials["default"];
}

void Material::release() {
	//Clean up materials
	for (auto kv : Material::materials) {
		delete kv.second;
	}
}