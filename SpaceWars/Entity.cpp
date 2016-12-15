#include "Entity.h"
#include "Octree.h"
#include "Scene.h"

Entity::Entity(Mesh* mesh, Material* material) : GameObject(mesh, material)
{
}


Entity::~Entity()
{
}

void Entity::update(float deltaTime)
{
	GameObject::update(deltaTime);
	//This is temporary until we have a proper updater
	physicsUpdate(Scene::getActive()->getEntityTree(), deltaTime);
}

void Entity::applyForce(Vector3 impulse)
{
	//This might include mass at some point, although unlikey in this game
	this->acceleration += impulse;
}

void Entity::physicsUpdate(Octree* octree, float deltaTime)
{
	auto& instancesToCheck = octree->nearbyObjects(this);
	for (auto obj : instancesToCheck)
	{
		if (this->collisionTest != ObjectType::Solid || obj->getType() != ObjectType::Solid)
			continue;

		if (obj == this || frameCollisions.find(this->getID()) != frameCollisions.end())
			continue;

		//checkCount++;
		std::shared_ptr<Collision> collision = obj->GetCollider()->isColliding(this->collider);

		if (collision != nullptr) {
			collision->collider1 = obj;
			collision->collider2 = this;
		}

		addFrameCollision(obj->getID(), collision);
		obj->addFrameCollision(getID(), collision);

		if (collision != nullptr && collision->colliding) {
			onCollision(collision->getEvent(this));
			obj->onCollision(collision->getEvent(obj));
		}
	}

	frameCollisions.clear();

	this->velocity = this->acceleration * deltaTime;
	this->getTransform()->Translate(this->velocity * deltaTime);

	this->velocity *= (1 - this->drag);
	acceleration = Vector3(0, 0, 0);
}