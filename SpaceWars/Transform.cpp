#include "Transform.h"



Transform::Transform()
{
	position = Vector3(0, 0, 0);
	scale = Vector3(1, 1, 1);
	rotation = Quaternion();
	dirty = true;
}


Transform::~Transform()
{
}

void Transform::Translate(float x, float y, float z)
{
	SetPosition(Vector3(position.x + x, position.y + y, position.z + z));
}

void Transform::Scale(float x, float y, float z)
{
	SetScale(Vector3(scale.x + x, scale.y + y, scale.z + z));
}

void Transform::Rotate(float x, float y, float z)
{
	SetRotation(Vector3(rotation.x + x, rotation.y + y, rotation.z + z));
}

void Transform::Translate(Vector3 translation)
{
	SetPosition(Vector3(position.x + translation.x, position.y + translation.y, position.z + translation.z));
}

void Transform::Scale(Vector3 scale)
{
	SetScale(Vector3(this->scale.x + scale.x, this->scale.y + scale.y, this->scale.z + scale.z));
}

void Transform::Rotate(Vector3 rotation)
{
	SetRotation(Vector3(this->rotation.x + rotation.x, this->rotation.y + rotation.y, this->rotation.z + rotation.z));
}

void Transform::Rotate(Quaternion rotation)
{
	SetRotation(this->rotation * rotation);
}

Vector3 Transform::GetPosition()
{
	return position;
}

Vector3 Transform::GetScale()
{
	return scale;
}

void Transform::SetPosition(Vector3 position)
{
	this->position = position;
	dirty = true;
}

void Transform::SetRotation(Vector3 rotation)
{
	this->rotation = Quaternion::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z);
	dirty = true;
}

void Transform::SetRotation(Quaternion orientation)
{
	this->rotation = orientation;
	dirty = true;
}

void Transform::SetScale(Vector3 scale)
{
	this->scale = scale;
	dirty = true;
}

void Transform::SetPosition(float x, float y, float z)
{
	SetPosition(Vector3(x, y, z));
}

void Transform::SetScale(float x, float y, float z)
{
	SetScale(Vector3(x, y, z));
}

void Transform::SetRotation(float x, float y, float z)
{
	SetRotation(Vector3(x, y, z));
}

Quaternion Transform::GetRotation() {
	return rotation;
}

Matrix Transform::GetMatrix()
{
	if (!dirty) {
		return matrix;
	}
	else {
		dirty = false;
		//I have absolutely no idea why the order of these matrices is in reverse
		//...possibly something to do with the transpose thing
		matrix = Matrix::Identity
			* Matrix::CreateScale(scale)
			* Matrix::CreateFromQuaternion(rotation)
			* Matrix::CreateTranslation(position);

		return matrix;
	}
}
