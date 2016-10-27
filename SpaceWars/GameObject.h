#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Collider.h"
#include <DirectXMath.h>

using namespace DirectX;

enum class ObjectType
{
	Default
};

class GameObject
{
	const int id;

	Mesh* mesh;
	Material* material;
	Transform* transform;
	Collider* collider;

	class OctNode* parent = nullptr;

protected:
	Collider* collider;
	//void scale(Vector3);
	Transform* transform;
	ObjectType collisionTest;
	bool hasFrameCollisions;
	std::unordered_map<int, std::shared_ptr<Collision>> frameCollisions;

public:
	GameObject(Mesh* mesh, Material* material);
	virtual ~GameObject();

	virtual void update(float deltaTime);

	//Return the mesh for this entity
	Mesh* getMesh();

	//Return this entity's transform
	Transform* getTransform() const;

	//Send data from this entity to the GPU
	void prepareMaterial(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix, ID3D11SamplerState* sampler);

	//Calculate the world matrix transpose for this entity
	DirectX::XMFLOAT4X4 getDrawMatrix();

	OctNode* getOctNode();
	void setOctNode(OctNode*);

	virtual ObjectType getType();

	/*
	Sets the origin of the game object, which indicates how
	far from the center rotations and scales will be performed
	*/
	const int getID();

	const Vector3 getPosition() const;

	const Quaternion getRotation() const;

	//void SetDebugColor(vector3 newColor);

	void rotate(Quaternion rotation);

	void translate(Vector3 translation);

	void scale(float scalar);

	void rotateTo(Vector3 orientation);

	void rotateTo(Quaternion orientation);

	virtual void update(float deltaTime);

	void addFrameCollision(int id, std::shared_ptr<Collision> collision);

	virtual void onCollision(const CollisionEvent&);

	void render(Matrix, Matrix);

	inline Collider* GetCollider()
	{
		return collider;
	}
};

