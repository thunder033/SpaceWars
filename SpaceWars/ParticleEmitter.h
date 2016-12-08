#pragma once
#include "Particle.hpp";
#include "DXResourceContext.h";
#include "GameObject.h";

using namespace DirectX;

struct ParticleEmitterDesc
{
	float energy = 100;
	int maxParticleCount = 100;
	float rate;
	float sizeDecay = 0;
	float speed = 1;
	Vector3 spread;
	Vector3 startVelocity;
	Transform* transform = nullptr;
};

class ParticleEmitter : GameObject
{
	int maxParticleCount;
	Particle** particles;
public:
	ParticleEmitter(const ParticleEmitterDesc &desc);
	~ParticleEmitter();
};

