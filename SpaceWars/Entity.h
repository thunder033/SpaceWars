#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include <DirectXMath.h>

class Entity
{
	Mesh* mesh;
	Material* material;
	Transform* transform;

public:
	Entity(Mesh* mesh, Material* material);
	virtual ~Entity();

	//Return the mesh for this entity
	Mesh* getMesh();

	//Return this entity's transform
	Transform* getTransform();

	//Send data from this entity to the GPU
	void prepareMaterial(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix, ID3D11SamplerState* sampler);

	//Calculate the world matrix transpose for this entity
	DirectX::XMFLOAT4X4 getDrawMatrix();
};

