/*----------------------------------------------
Derived from: Alberto Bobadilla (labigm@gmail.com)
Modified by: Greg Rozmarynowycz
Date: 2015/10
----------------------------------------------*/
#pragma once

#include "Transform.h"
#include "Collision.hpp"
#include <vector>

//System Class
class Collider
{
	float radius = 0.0f; //Radius of the Bounding Sphere
	Vector3 lastCollision;
	Transform* transform;

public:
	/*
	Collider
	USAGE: Constructor
	ARGUMENTS: ---
	OUTPUT: class object
	*/
	Collider(std::vector<Vector3> a_lVectorList, Transform* transform);
	/*
	Collider
	USAGE: Copy Constructor
	ARGUMENTS: class object to copy
	OUTPUT: class object instance
	*/
	Collider(Collider const& other);
	/*
	operator=
	USAGE: Copy Assignment Operator
	ARGUMENTS: class object to copy
	OUTPUT: ---
	*/
	Collider& operator=(Collider const& other);
	/*
	~Collider
	USAGE: Destructor
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	~Collider(void);

	/*
	Swap
	USAGE: Changes object contents for other object's
	ARGUMENTS:
	other -> object to swap content from
	OUTPUT: ---
	*/
	void Swap(Collider& other);

	/*
	GetCenter
	USAGE: Gets the Bounding Sphere's center in world coordinates
	ARGUMENTS: ---
	OUTPUT: vector3 -> Center's of the sphere in world coordinates
	*/
	Vector3 getCenter(void);

	/*
	GetRadius
	USAGE: Gets the Bounding Sphere's radius
	ARGUMENTS: ---
	OUTPUT: float -> radius of the Bounding Sphere
	*/
	float getRadius(void);

	Vector3 getLastCollision();

	/*
	IsColliding
	USAGE: Asks if there is a collision with another Bounding sphere Object
	ARGUMENTS:
	Collider* const a_pOther -> Other object to check collision with
	OUTPUT: bool -> check of the collision
	*/
	std::shared_ptr<Collision> isColliding(Collider* const a_pOther);

private:
	/*
	Release
	USAGE: Deallocates member fields
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void release(void);
};