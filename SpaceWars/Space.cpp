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
	entities.push_back(new Entity(Mesh::getByIndex(0), Material::getDefault()));
	entities.push_back(new Entity(Mesh::getByIndex(1), Material::getByName("crate")));
	entities.push_back(new Entity(Mesh::getByIndex(2), Material::getDefault()));
	entities.push_back(new Entity(Mesh::getByIndex(3), Material::getByName("blue")));
	entities.push_back(new Entity(Mesh::getByIndex(4), Material::getDefault()));
	entities.push_back(new Entity(Mesh::getByIndex(5), Material::getByName("blue")));

	//move the first mesh to an arbitrary position
	entities[0]->getTransform()->SetPosition(1.5f, 0, 0);
}

void Space::update(float deltaTime, float totalTime)
{
	//Make the cone orbit
	entities[0]->getTransform()->SetPosition(cos(totalTime), sin(totalTime), 0);
	entities[0]->getTransform()->SetRotation(0, 0, totalTime);

	//Spinning Cube
	entities[1]->getTransform()->SetPosition(0, sin(totalTime), -2);
	entities[1]->getTransform()->SetRotation(sin(totalTime) * 3.14f, sin(totalTime) * 3.14f, sin(totalTime) * 3.14f);

	//Move stuff out of the way
	entities[2]->getTransform()->SetPosition(-2, 0, 0);
	entities[3]->getTransform()->SetPosition(-4, 0, 0);
	entities[4]->getTransform()->SetPosition(4, 0, 0); //sphere

	//Make the Torus spin
	entities[5]->getTransform()->SetPosition(4, 0, 0);
	entities[5]->getTransform()->SetRotation(totalTime * 3, 0, totalTime * 3);
	entities[5]->getTransform()->SetScale(2, 2, 2);
}

void Space::draw(float deltaTime, float totalTime, Renderer* renderer) {

}