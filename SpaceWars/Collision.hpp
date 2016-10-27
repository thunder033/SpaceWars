#pragma once

class GameObject;

struct CollisionEvent {
	Vector3 intersectPt;
	Vector3 penetrationVector;
	Vector3 collideeIntersectPt;
	GameObject* collidee;
};

struct Collision {
	Vector3 intersectPoint1;
	Vector3 penetrationVector;
	Vector3 intersectPoint2;

	GameObject* collider1;
	GameObject* collider2;
	bool colliding;

	const CollisionEvent getEvent(const GameObject* obj) {
		CollisionEvent evt = CollisionEvent();
		if (obj == collider1) {
			evt.collidee = collider2;
			evt.intersectPt = intersectPoint1;
			evt.collideeIntersectPt = intersectPoint2;
			evt.penetrationVector = penetrationVector;
		}
		else {
			evt.collidee = collider1;
			evt.intersectPt = intersectPoint2;
			evt.collideeIntersectPt = intersectPoint1;
			evt.penetrationVector = -penetrationVector;
		}
		return evt;
	}
};