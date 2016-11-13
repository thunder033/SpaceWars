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

void Material::prepare(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix, DirectX::XMFLOAT4X4 drawMatrix, ID3D11SamplerState* sampler) {

	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	vertexShader->SetMatrix4x4("world", drawMatrix);
	vertexShader->SetMatrix4x4("view", viewMatrix);
	vertexShader->SetMatrix4x4("projection", projectionMatrix);

	pixelShader->SetSamplerState("Sampler", sampler);

	pixelShader->SetData("Color", &getColor(), sizeof(DirectX::XMFLOAT4));
	pixelShader->SetShaderResourceView("Texture", getTexture());

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	pixelShader->CopyAllBufferData();
	vertexShader->CopyAllBufferData();

	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	pixelShader->SetShader();
	vertexShader->SetShader();
}