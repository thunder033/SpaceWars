#pragma once
#include "Particle.hpp";
#include "DXResource.h";
#include "GameObject.h";

using namespace DirectX;

struct ParticleEmitterDesc
{
	// How many seconds a particle will live
	float energy = 100; 
	// Maximum number of particles active at any time
	int maxParticleCount = 100;
	// How many particles to (attempt to) spawn per second
	float rate = 0;
	// The rate at which particles will decrease in size
	float sizeDecay = 0;
	// How fast the particles will move
	float speed = 1;
	// What directions the particles will move in 
	Vector3 spread = Vector3(1.0f);
	// Start velocity applied to each particle
	Vector3 startVelocity = Vector3(0.0f);
	// Transform for the whole emitter
	Transform* transform = nullptr;
	// The material to apply to the spawner (probably "particle")
	Material* material = nullptr;
};

#define PARTICLE_EMITTER_UNIFORM -1
class ParticleEmitter : public GameObject, DXResource
{
	int maxParticleCount;
	Particle** particles;
	ParticleVertex* particleVertices;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Index where the next particle will spawn
	unsigned int headPosition = 0;
	// Index where the next particle will die
	unsigned int tailPosition = 0;
	unsigned int liveParticleCount = 0;
	// How many particles will spawn per second
	float rate;

	// Seconds elapsed since last particle was emitted
	float emittedElapsed;
	// Elapsed time that will trigger another spawn
	float emissionTrigger;
	// "Prototype" for each new particle
	ParticleDesc particleDesc;

	void copyParticle(int index);
public:
	ParticleEmitter(const ParticleEmitterDesc &desc);
	~ParticleEmitter();

	bool canEmit();

	void emit();

	void update(float deltaTime);

	void initBuffers();

	void copyBuffers(ID3D11DeviceContext* context);

	ID3D11Buffer* getVertexBuffer();

	ID3D11Buffer* getIndexBuffer();

	Particle** getParticles();

	unsigned int& getHeadPosition();

	unsigned int& getTailPosition();

	int& getMaxParticleCount();
};

