#include "DXCore.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

#pragma once
class Transform
{
	Vector3 position;
	Vector3 scale;
	Vector3 rotation;

	Matrix matrix;
	bool dirty;

public:
	Transform();
	~Transform();

	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void Scale(float x, float y, float z);

	/*void Translate(DirectX::XMVECTOR translation);
	void Rotate(DirectX::XMVECTOR rotation);
	void Scale(DirectX::XMVECTOR scale);*/

	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void SetPosition(Vector3 position);
	void SetScale(Vector3 scale);
	void SetRotation(Vector3 rotation);

	Vector3 GetPosition();
	Vector3 GetScale();
	Vector3 GetRotation();

	Quaternion GetOrientation();

	Matrix GetMatrix();
};

