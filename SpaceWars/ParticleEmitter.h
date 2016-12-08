#pragma once
#include "Particle.hpp";
#include "DXResourceContext.h";
#include "GameObject.h";

using namespace DirectX;

struct ParticleEmitterDesc
{
	float energy = 100;
	int maxParticleCount = 100;
	float rate = 0;
	float sizeDecay = 0;
	float speed = 1;
	Vector3 spread = Vector3(1.0f);
	Vector3 startVelocity = Vector3(0.0f);
	Transform* transform = nullptr;
};

#define PARTICLE_EMITTER_UNIFORM -1
class ParticleEmitter : GameObject
{
	int maxParticleCount;
	Particle** particles;

	int emitPosition = 1;
	int endPosition = 0;
	float rate;

	float emittedElapsed;
	float emissionTrigger;
	ParticleDesc particleDesc;
public:
	ParticleEmitter(const ParticleEmitterDesc &desc);
	~ParticleEmitter();

	bool canEmit();

	void emit();

	void draw();

	void update(const float &deltaTime);
};

