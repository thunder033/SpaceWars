#include "DXCore.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

#pragma once
class Transform
{
	Vector3 position;
	Vector3 scale;
	Quaternion rotation;

	Matrix matrix;
	bool dirty;

public:
	Transform();
	~Transform();

	//Transform by values
	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void Scale(float x, float y, float z);

	//Transform by vectors
	void Translate(Vector3 translation);
	void Rotate(Vector3 rotation);
	void Rotate(Quaternion rotation);
	void Scale(Vector3 scale);

	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void SetPosition(Vector3 position);
	void SetScale(Vector3 scale);
	void SetRotation(Vector3 orientation);
	void SetRotation(Quaternion orientation);

	Vector3 GetPosition();
	Vector3 GetScale();
	Quaternion GetRotation();

	Matrix GetMatrix();
};

