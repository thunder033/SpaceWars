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

	//update camera properties
	void update(double deltaTime, double elapsedTime);

	void setAspectRatio(float aspectRatio);

	//update the view matrix if it's stale
	void updateViewMatrix();

	//Move along local Z axis
	void advance(float units);

	//Move along local X axis
	void strafe(float units);

	//Move along local Y axis
	void ascend(float units);

	//Rotate by X/Y radians
	void rotate(float x, float y);

	Transform* getTransform() {
		return transform;
	}

	DirectX::XMFLOAT4X4 getViewMatrix() {
		return viewMatrix;
	}

	DirectX::XMFLOAT4X4 getProjectionMatrix() {
		return projectionMatrix;
	}
};

