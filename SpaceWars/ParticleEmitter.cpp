#include "ParticleEmitter.h"


ParticleEmitter::ParticleEmitter(const ParticleEmitterDesc & desc)
	:GameObject(nullptr, nullptr)
{
	if (desc.transform != nullptr) {
		this->transform = desc.transform;
	}

	maxParticleCount = desc.maxParticleCount;

	particles = new Particle*[maxParticleCount];

	// Maybe transforms array here

	rate = (desc.rate == PARTICLE_EMITTER_UNIFORM) ? 1000 / (desc.energy / maxParticleCount) : desc.rate;
	
	emittedElapsed = 0;
	emissionTrigger = 1000 / rate;

	particleDesc = {};
	particleDesc.startEnergy = desc.energy;
	particleDesc.sizeDecay = desc.sizeDecay;
	particleDesc.speed = desc.speed;
	particleDesc.startVelocity = desc.startVelocity;
	particleDesc.spread = desc.spread;
}

ParticleEmitter::~ParticleEmitter()
{
}

bool ParticleEmitter::canEmit()
{
	return emitPosition != endPosition;
}

void ParticleEmitter::emit()
{
	if (canEmit()) 
	{
		int &p = emitPosition;
		if (particles[p] == nullptr)
		{
			particles[p] = new Particle(particleDesc);
		}

		particles[p]->spawn(transform->GetPosition());

		if (++emitPosition >= maxParticleCount)
		{
			emitPosition = 0;
		}
	}
}

void ParticleEmitter::update(const float & deltaTime)
{
	//if this emitter automatically emits
	if (rate > 0)
	{
		emittedElapsed += deltaTime;
		if (emittedElapsed > emissionTrigger)
		{
			emittedElapsed = 0;
			emit();
		}
	}

	int count = maxParticleCount;
	for (int p = endPosition, int c = 0; c < count; p++, c++)
	{
		if (p >= maxParticleCount)
		{
			p = 0;
		}

		if (particles[p] == nullptr)
		{
			continue;
		}

		particles[p]->update(deltaTime);
		// kill any particle out of energy
		// this is triggered here so the system can track it's particles
		// instead of in the particle itself
		if (particles[p]->shouldDie())
		{
			particles[p]->die();

			if (++endPosition >= maxParticleCount)
			{
				endPosition = 0;
			}
		}
	}
}
