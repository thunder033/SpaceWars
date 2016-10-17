#pragma once
#include "DXCore.h"
#include "Transform.h"
#include <DirectXMath.h>

class Camera
{
	float aspectRatio;
	Transform* transform;

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	Vector3 getForward();
public:
	Camera(float aspectRatio);
	~Camera();

	void Update(double deltaTime, double elapsedTime);

	void setAspectRatio(float aspectRatio);

	void updateViewMatrix();

	void advance(float units);

	void strafe(float units);

	void ascend(float units);

	void rotate(float x, float y);

	DirectX::XMFLOAT4X4 getViewMatrix() {
		return viewMatrix;
	}

	DirectX::XMFLOAT4X4 getProjectionMatrix() {
		return projectionMatrix;
	}
};

