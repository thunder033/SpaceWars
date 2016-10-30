#pragma once

#include "GameObject.h"

//class Entity;
class Entity : public GameObject
{
	Vector3 velocity;
	Vector3 acceleration;

	float drag = 0;

public:
	Entity();
	Entity(Mesh* mesh, Material* material);
	virtual ~Entity();

	virtual void update(float deltaTime);
	virtual void physicsUpdate(class Octree* octree, float delatTime);

	void applyForce(Vector3 impulse);
};