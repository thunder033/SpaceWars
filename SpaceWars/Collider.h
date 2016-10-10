///*----------------------------------------------
//Derived from: Alberto Bobadilla (labigm@gmail.com)
//Modified by: Greg Rozmarynowycz
//Date: 2015/10
//----------------------------------------------*/
//#pragma once
//
//#include "OBB.hpp"
//#include "Transform.h"
//#include "Projection.hpp"
//#include "Collision.hpp"
//#include <vector>
//
//#include <SimpleMath.h>
//using namespace DirectX::SimpleMath;
//
//enum class ColliderType
//{
//	Sphere = 0,
//	OBB
//};
////System Class
//class Collider
//{
//	float radius = 0.0f; //Radius of the Bounding Sphere
//	Vector3 centroid = Vector3(0.0f); //Will store the center point of the Sphere Class
//	Vector3 min = Vector3(0.0f); //Will store the minimum vector of the Sphere Class
//	Vector3 max = Vector3(0.0f); //Will store the maximum vector of the Sphere Class
//	Vector3 size = Vector3(0.0f);
//	Vector3 alignedSize = Vector3(0.0f);
//	Vector3 lastCollision;
//	Transform* transform;
//	OBB obb;
//	ColliderType type = ColliderType::OBB;
//
//	void GetMinMax(Vector3& min, Vector3& max, std::vector<Vector3> points);
//public:
//	/*
//	Collider
//	USAGE: Constructor
//	ARGUMENTS: ---
//	OUTPUT: class object
//	*/
//	Collider(std::vector<Vector3> a_lVectorList, Transform* transform);
//	/*
//	Collider
//	USAGE: Copy Constructor
//	ARGUMENTS: class object to copy
//	OUTPUT: class object instance
//	*/
//	Collider(Collider const& other);
//	/*
//	operator=
//	USAGE: Copy Assignment Operator
//	ARGUMENTS: class object to copy
//	OUTPUT: ---
//	*/
//	Collider& operator=(Collider const& other);
//	/*
//	~Collider
//	USAGE: Destructor
//	ARGUMENTS: ---
//	OUTPUT: ---
//	*/
//	~Collider(void);
//
//	/*
//	Swap
//	USAGE: Changes object contents for other object's
//	ARGUMENTS:
//	other -> object to swap content from
//	OUTPUT: ---
//	*/
//	void Swap(Collider& other);
//
//	/*
//	GetCenter
//	USAGE: Gets the Bounding Sphere's center in world coordinates
//	ARGUMENTS: ---
//	OUTPUT: vector3 -> Center's of the sphere in world coordinates
//	*/
//	Vector3 GetCenter(void);
//
//	/*
//	GetRadius
//	USAGE: Gets the Bounding Sphere's radius
//	ARGUMENTS: ---
//	OUTPUT: float -> radius of the Bounding Sphere
//	*/
//	float GetRadius(void);
//
//	Vector3 GetSize(void);
//	Matrix GetAxisAlignedTransform(void);
//	Vector3 GetMin();
//	Vector3 GetMax();
//	std::vector<Vector3> CalculateOBB();
//	void setType(ColliderType);
//	void calculateAABB();
//	Vector3 GetLastCollision();
//	OBB UpdateOBB();
//	const OBB GetOBB();
//	Vector3 GetAABBMin();
//	Vector3 GetAABBMax();
//
//	/*
//	IsColliding
//	USAGE: Asks if there is a collision with another Bounding sphere Object
//	ARGUMENTS:
//	Collider* const a_pOther -> Other object to check collision with
//	OUTPUT: bool -> check of the collision
//	*/
//	std::shared_ptr<Collision> IsColliding(Collider* const a_pOther);
//
//private:
//	/*
//	Release
//	USAGE: Deallocates member fields
//	ARGUMENTS: ---
//	OUTPUT: ---
//	*/
//	void Release(void);
//	/*
//	Init
//	USAGE: Allocates member fields
//	ARGUMENTS: ---
//	OUTPUT: ---
//	*/
//	void Init(void);
//};