#pragma once
#include "Transform.h";

struct ParticleDesc
{
	float sizeDecay = 0;
	float speed = 0;
	float startEnergy;
	Vector3 startVelocity;
	Vector3 spread;
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

		velocity = Vector3(0.0);
		acceleration = Vector3(0.0);
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

	void spawn(const Vector3 &position)
	{
		active = true;
		energy = startEnergy;
		transform->SetPosition(position);
		transform->SetScale(1);

		velocity = Vector3(
			//Set the particle off in a random direction
			0.5 - ((double) rand() / (RAND_MAX)) + 1, 
			0.5 - ((double)rand() / (RAND_MAX)) + 1,
			0.5 - ((double)rand() / (RAND_MAX)) + 1) 
			//Apply the parameters to determin final direction
			* spread * speed + startVelocity;
	}

	float getEnergy()
	{
		return energy;
	}

	void update(const float &deltaTime) {
		velocity += acceleration * deltaTime;
		transform->Translate(velocity * deltaTime);
		transform->SetScale(1.0f - sizeDecay);

		energy -= deltaTime;
	}
};