#pragma once
#include "Particle.hpp";
#include "DXResource.h";
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

	unsigned int headPosition = 0;
	unsigned int tailPosition = 0;
	unsigned int liveParticleCount = 0;
	float rate;

	float emittedElapsed;
	float emissionTrigger;
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

