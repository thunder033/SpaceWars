#pragma once
#include "Transform.h";

struct ParticleDesc
{
	float sizeDecay = 0;
	float speed = 0;
	float startEnergy;
	Vector3 spawnPosition;
	Vector3 startVelocity;
	Vector3 spread;
};

struct ParticleVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT4 color;
	float size;
};

class Particle 
{
	Transform* transform;
	Vector3 velocity;
	Vector3 acceleration;
	float energy;

	float &sizeDecay;
	float &speed;

	bool active;
	float &startEnergy;
	Vector3 &startVelocity;
	Vector3 &spread;
public:

	Particle(ParticleDesc &desc):
		sizeDecay(desc.sizeDecay), 
		speed(desc.speed),
		startEnergy(desc.startEnergy),
		startVelocity(desc.startVelocity),
		spread(desc.spread)
	{
		transform = new Transform();
		transform->SetPosition(desc.spawnPosition);

		velocity = Vector3::Zero;
		acceleration = Vector3::Zero;
		active = true;
	}

	~Particle() 
	{
		if (transform != nullptr) 
		{
			delete transform;
			transform = nullptr;
		}
	}

	void die() 
	{
		active = false;
		transform->SetScale(0, 0, 0);
	}

	// resurrects the particle in a new position with random velocity
	void spawn(const Vector3 &position)
	{
		active = true;
		energy = startEnergy;
		transform->SetPosition(position);
		transform->SetScale(1);

		velocity = Vector3(
			//Set the particle off in a random direction
			0.5 - ((double)rand() / (RAND_MAX)), 
			0.5 - ((double)rand() / (RAND_MAX)),
			0.5 - ((double)rand() / (RAND_MAX))) 
			//Apply the parameters to determin final direction
			* spread * speed + startVelocity;
	}

	// indicates if it's time for the particle to meet its maker
	bool shouldDie()
	{
		return energy <= 0 && active == true;
	}

	void update(const float &deltaTime) {
		velocity += acceleration * deltaTime;
		transform->Translate(velocity * deltaTime);
		transform->Scale(Vector3(1.0f - sizeDecay));

		energy -= deltaTime;
	}

	Vector3 const getPosition()
	{
		return transform->GetPosition();
	}

	float const getSize()
	{
		return transform->GetScale().x;
	}
};