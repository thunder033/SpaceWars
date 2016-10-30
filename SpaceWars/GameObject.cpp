#include "GameObject.h"

int GameObject::autoIncrementId = 0;

GameObject::GameObject(Mesh* mesh, Material* material) : id(autoIncrementId++)
{
	this->mesh = mesh;
	this->material = material;
	transform = new Transform();
	this->collider = new Collider(mesh->getVertices(), transform);
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

inline void GameObject::rotate(Quaternion rotation)
{
	transform->Rotate(rotation);
}

inline void GameObject::translate(Vector3 translation)
{
	transform->Translate(translation);
}

inline void GameObject::scale(float scalar)
{
	transform->Scale(Vector3(scalar));
}

inline void GameObject::rotateTo(Vector3 orientation)
{
	transform->SetRotation(orientation);
}

inline void GameObject::rotateTo(Quaternion orientation)
{
	transform->SetRotation(orientation);
}

void GameObject::prepareMaterial(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix, ID3D11SamplerState* sampler)
{
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	material->getVertexShader()->SetMatrix4x4("world", getDrawMatrix());
	material->getVertexShader()->SetMatrix4x4("view", viewMatrix);
	material->getVertexShader()->SetMatrix4x4("projection", projectionMatrix);

	material->getPixelShader()->SetData("Color", &material->getColor(), sizeof(DirectX::XMFLOAT4));

	material->getPixelShader()->SetSamplerState("Sampler", sampler);
	material->getPixelShader()->SetShaderResourceView("Texture", material->getTexture());

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	material->getVertexShader()->CopyAllBufferData();
	material->getPixelShader()->CopyAllBufferData();

	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	material->getVertexShader()->SetShader();
	material->getPixelShader()->SetShader();

}

void GameObject::addFrameCollision(int id, std::shared_ptr<Collision> collision)
{
	frameCollisions[id] = collision;
}

void GameObject::onCollision(const CollisionEvent &)
{
}
