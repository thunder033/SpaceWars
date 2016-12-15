#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(const ParticleEmitterDesc & desc)
	:GameObject(nullptr, desc.material)
{
	if (desc.transform != nullptr) {
		this->transform = desc.transform;
	}

	collisionTest = ObjectType::ParticleEmitter;
	maxParticleCount = desc.maxParticleCount;

	particles = new Particle*[maxParticleCount];
	std::fill(particles, particles + maxParticleCount, static_cast<Particle*>(NULL));

	// Maybe transforms array here

	rate = (desc.rate == PARTICLE_EMITTER_UNIFORM) ? 1.0f / (desc.energy / (float)maxParticleCount) : desc.rate;
	
	emittedElapsed = 0;
	emissionTrigger = 1.0f / rate;

	particleDesc = {};
	particleDesc.startEnergy = desc.energy;
	particleDesc.sizeDecay = desc.sizeDecay;
	particleDesc.speed = desc.speed;
	particleDesc.startVelocity = desc.startVelocity;
	particleDesc.spread = desc.spread;

	particleVertices = new ParticleVertex[4 * maxParticleCount];
	for (int i = 0; i < maxParticleCount * 4; i += 4)
	{
		particleVertices[i + 0].uv = XMFLOAT2(0, 0);
		particleVertices[i + 1].uv = XMFLOAT2(1, 0);
		particleVertices[i + 2].uv = XMFLOAT2(1, 1);
		particleVertices[i + 3].uv = XMFLOAT2(0, 1);
	}

	this->initBuffers();
}

ParticleEmitter::~ParticleEmitter()
{
	delete[] particles;
	delete[] particleVertices;
	vertexBuffer->Release();
	indexBuffer->Release();
}

void ParticleEmitter::initBuffers()
{

	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticleCount;
	mRC->mDevice->CreateBuffer(&vbDesc, 0, &vertexBuffer);

	unsigned int* indices = new unsigned int[maxParticleCount * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticleCount * 4; i += 4)
	{
		//Make a quad
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * maxParticleCount * 6;
	mRC->mDevice->CreateBuffer(&ibDesc, &indexData, &indexBuffer);

	delete[] indices;
}

void ParticleEmitter::copyParticle(int index)
{
	int i = index * 4;

	Vector3 pos = particles[index]->getPosition();
	particleVertices[i + 0].position = pos;
	particleVertices[i + 1].position = pos;
	particleVertices[i + 2].position = pos;
	particleVertices[i + 3].position = pos;

	Vector4 color = Vector4(1, 0, 0, 1);
	particleVertices[i + 0].color = color;
	particleVertices[i + 1].color = color;
	particleVertices[i + 2].color = color;
	particleVertices[i + 3].color = color;

	float size = particles[index]->getSize() * this->transform->GetScale().x;
	particleVertices[i + 0].size = size;
	particleVertices[i + 1].size = size;
	particleVertices[i + 2].size = size;
	particleVertices[i + 3].size = size;
}

void ParticleEmitter::copyBuffers(ID3D11DeviceContext* context)
{
	for (int p = tailPosition, c = 0; c < liveParticleCount; c++, p = (p + 1) % maxParticleCount)
	{
		copyParticle(p);
	}

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	memcpy(mapped.pData, particleVertices, sizeof(ParticleVertex) * 4 * maxParticleCount);

	context->Unmap(vertexBuffer, 0);
}

ID3D11Buffer* ParticleEmitter::getVertexBuffer()
{
	return vertexBuffer;
}

ID3D11Buffer* ParticleEmitter::getIndexBuffer()
{
	return indexBuffer;
}

unsigned int& ParticleEmitter::getHeadPosition()
{
	return headPosition;
}

unsigned int& ParticleEmitter::getTailPosition()
{
	return tailPosition;
}

int& ParticleEmitter::getMaxParticleCount()
{
	return maxParticleCount;
}

Particle** ParticleEmitter::getParticles()
{
	return particles;
}

bool ParticleEmitter::canEmit()
{
	return headPosition != tailPosition || particles[0] == nullptr;
}

void ParticleEmitter::emit()
{
	if (canEmit()) 
	{
		unsigned int &p = headPosition;
		if (particles[p] == nullptr)
		{
			particles[p] = new Particle(particleDesc);
		}

		particles[p]->spawn(transform->GetPosition());
		liveParticleCount++;

		if (++headPosition >= maxParticleCount)
		{
			headPosition = 0;
		}
	}
}

void ParticleEmitter::update(float deltaTime)
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

	for (int p = tailPosition, c = 0; c < maxParticleCount; p++, c++)
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
			liveParticleCount--;

			if (++tailPosition >= maxParticleCount)
			{
				tailPosition = 0;
			}
		}
	}

	GameObject::update(deltaTime);
}
