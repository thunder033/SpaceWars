#include "Collider.h"

//  Collider
void Collider::init(void)
{
	radius = 0.0f;
	origin = Vector3(0.0f);
	min = Vector3(0.0f);
	max = Vector3(0.0f);
}
void Collider::swap(Collider& other)
{
	std::swap(radius, other.radius);
	std::swap(transform, other.transform);

	std::swap(origin, other.origin);
	std::swap(min, other.min);
	std::swap(max, other.max);
}
void Collider::release(void)
{
	if (transform != nullptr)
	{
		delete transform;
		transform = nullptr;
	}
}

void Collider::getMinMax(Vector3& min, Vector3& max, std::vector<Vector3> points) {
	if (points.size() == 0)
		return;

	min = points[0];
	max = points[0];

	std::vector<Vector3>::iterator it;
	for (it = points.begin(); it != points.end(); ++it)
	{
		if (it->x < min.x)
			min.x = it->x;
		else if (it->x > max.x)
			max.x = it->x;

		if (it->y < min.y)
			min.y = it->y;
		else if (it->y > max.y)
			max.y = it->y;

		if (it->z < min.z)
			min.z = it->z;
		else if (it->z > max.z)
			max.z = it->z;
	}
}

//The big 3
Collider::Collider(std::vector<Vector3> a_lVectorList, Transform* transform)
{
	this->transform = transform;
	getMinMax(min, max, a_lVectorList);
	origin = (min + max) / 2.0f;
	radius = Vector3::Distance(origin, max);
}
Collider::Collider(Collider const& other)
{
	radius = other.radius;
	transform = other.transform;

	origin = other.origin;
	min = other.min;
	max = other.max;
}
Collider& Collider::operator=(Collider const& other)
{
	if (this != &other)
	{
		release();
		init();
		Collider temp(other);
		swap(temp);
	}
	return *this;
}
Collider::~Collider() { release(); };

//Accessors
Vector3 Collider::getCenter(void) { 
	return Vector3();

	//TODO: DX center calculation
	//return Vector3(transform->GetMatrix() * Vector4(origin, 1.0f)); 
}

float Collider::getRadius(void) {
	Vector3 scale = transform->GetScale();
	return radius * std::max(std::max(scale.x, scale.y), scale.z);
}

Vector3 Collider::getLastCollision()
{
	return lastCollision;
}

Vector3 Collider::getMin() {
	return Vector3();
	
	//TODO: DX min calulation
	//return Vector3(transform->GetMatrix()[3] + Vector4(min, 1.0f));
}

Vector3 Collider::getMax() {
	return Vector3();

	//TODO: DX Max Calculation
	//return Vector3(transform->GetMatrix()[3] + Vector4(max, 1.0f));
}

std::shared_ptr<Collision> Collider::isColliding(Collider* const other)
{
	float dist = Vector3::Distance(getCenter(), other->getCenter());
	if (dist > (getRadius() + other->getRadius())) {
		return nullptr;
	}

	std::shared_ptr<Collision> collision(new Collision());
	collision->colliding = true;
	return collision;
}