#pragma once
#include "DXCore.h"
#include "Transform.h"
#include <DirectXMath.h>

class Camera
{
	float aspectRatio;
	Transform* transform;

	bool	isDirtyCameraView,
			isDirtyCameraProjection;

	DirectX::XMFLOAT4X4 matrixView;
	DirectX::XMFLOAT4X4 matrixProjection;
	/*
		Vector3 getForward(); 
		is this a helper function? or a function that is more important than as a helper function? hprGetForward() hprNameOfFunction would be great.
		prefixs help to understand what the function purpose is 
	*/
public:
	Camera(float aspectRatio);
	

	/*
		void Update(double deltaTime, double elapsedTime);
			If camera class was to be part of "game object", which is what you seem to be doing because you extended from transform, 
			it has to as well extend a class that has update method rather than having its own

		void Advance(float units);
		void Strafe(float units);
		void Ascend(float units);
		void Rotate(float x, float y);
			These functions are basically the same in that they transform the transform class.

	*/

	void SetAspectRatio(float aspectRatio);

	/*
		void UpdateViewMatrix();
		try what Transform class did. 
	*/

	DirectX::XMFLOAT4X4 getViewMatrix();

	DirectX::XMFLOAT4X4 getProjectionMatrix();
};

