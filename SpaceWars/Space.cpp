#include "Space.h"



Space::Space() : Scene("space")
{
}


Space::~Space()
{
	
}

void Space::init()
{
	//Create entities to be rendered
	entities.push_back(new Entity(Mesh::getByIndex(6), Material::getDefault()));
	entities.push_back(new Entity(Mesh::getByIndex(1), Material::getByName("crate")));
	entities.push_back(new Entity(Mesh::getByIndex(2), Material::getDefault()));
	entities.push_back(new Entity(Mesh::getByIndex(3), Material::getByName("blue")));
	entities.push_back(new Entity(Mesh::getByIndex(4), Material::getDefault()));
	entities.push_back(new Entity(Mesh::getByIndex(5), Material::getByName("blue")));

	//move the first mesh to an arbitrary position
	entities[0]->getTransform()->SetPosition(1.5f, 0, 0);

	entities[4]->getTransform()->SetPosition(4, 0, 0); //sphere

	//Fudge the radius for spherical-ish objects
	entities[4]->GetCollider()->setRadius(.5f);
	entities[5]->GetCollider()->setRadius(.7f);

	ParticleEmitterDesc emitterDesc = {};
	emitterDesc.maxParticleCount = 500;
	emitterDesc.energy = 6;
	emitterDesc.material = Material::getByName("particle");
	emitterDesc.rate = 60.0f;
	emitterDesc.speed = 2.0f;
	emitterDesc.sizeDecay = 0.03f;
	emitterDesc.transform = new Transform();
	emitterDesc.transform->SetScale(0.2f);

	entities.push_back(new ParticleEmitter(emitterDesc));

	Scene::init();
}

void Space::update(float deltaTime, float totalTime)
{
	//Make the cone orbit
	entities[0]->getTransform()->SetPosition(cos(totalTime), sin(totalTime), 0);
	entities[0]->getTransform()->SetRotation(1.57, -totalTime + 1.57, 0);

	//Spinning Cube
	entities[1]->getTransform()->SetPosition(0, sin(totalTime), -2);
	entities[1]->getTransform()->SetRotation(sin(totalTime) * 3.14f, sin(totalTime) * 3.14f, sin(totalTime) * 3.14f);

	//Move stuff out of the way
	entities[2]->getTransform()->SetPosition(-2, 0, 0);
	entities[3]->getTransform()->SetPosition(-4, 0, 0);

	//Make the Torus spin
	entities[5]->getTransform()->SetPosition(4, 0, 0);
	entities[5]->getTransform()->SetRotation(totalTime * 3, 0, totalTime * 3);
	entities[5]->getTransform()->SetScale(2, 2, 2);

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		entities[4]->translate(Vector3(deltaTime, 0, 0));
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		entities[4]->translate(Vector3(-deltaTime, 0, 0));
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		entities[4]->translate(Vector3(0, deltaTime, 0));
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		entities[4]->translate(Vector3(0, -deltaTime, 0));
	}

	Scene::update(deltaTime, totalTime);
}

void Space::draw(float deltaTime, float totalTime, Renderer* renderer) {

}