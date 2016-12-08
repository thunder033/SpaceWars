#include "GameObject.h"

int GameObject::autoIncrementId = 0;

GameObject::GameObject(Mesh* mesh, Material* material) : id(autoIncrementId++)
{
	this->mesh = mesh;
	this->material = material;
	transform = new Transform();

	this->collider = nullptr;
	if (mesh != nullptr) {
		this->collider = new Collider(mesh->getVertices(), transform);
	}
	
}


GameObject::~GameObject()
{
	if (transform != nullptr) {
		delete transform;
		transform = nullptr;
	}

	delete collider;
}

void GameObject::update(float deltaTime)
{
	frameMaterial = material;
}

Mesh * GameObject::getMesh()
{
	return mesh;
}

Transform* GameObject::getTransform() const
{
	return transform;
}

DirectX::XMFLOAT4X4 GameObject::getDrawMatrix()
{
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform->GetMatrix());
	DirectX::XMFLOAT4X4 matrix;
	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixTranspose(W)); // Transpose for HLSL!
	return matrix;
}

OctNode * GameObject::getOctNode()
{
	return parent;
}

void GameObject::setOctNode(OctNode * parent)
{
	this->parent = parent;
}

ObjectType GameObject::getType()
{
	return collisionTest;
}

const int GameObject::getID()
{
	return id;
}

const Vector3 GameObject::getPosition() const
{
	return transform->GetPosition();
}

const Quaternion GameObject::getRotation() const
{
	return transform->GetRotation();
}

Material* GameObject::getMaterial()
{
	return frameMaterial;
}

void GameObject::addFrameCollision(int id, std::shared_ptr<Collision> collision)
{
	frameCollisions[id] = collision;
}

void GameObject::onCollision(const CollisionEvent &)
{
#if defined(DEBUG) || defined(_DEBUG)
		frameMaterial = Material::getByName("green");
#endif
}
