#include "Scene.h"

Scene* Scene::activeScene = nullptr;
std::map<std::string, Scene*> Scene::scenes;

Scene::Scene(std::string name)
{
	this->name = name;
	Scene::scenes[name] = this;
}


Scene::~Scene()
{
}

void Scene::init()
{
	octree = new Octree(Vector3(-10.0f, -10.0f, -10.0f), Vector3(10.0f, 10.0f, 10.0), 1);
	octree->generateTree(entities);

}

void Scene::update(float dt, float tt)
{
	for (auto entity : entities)
	{
		entity->update(dt);
	}
};

void Scene::draw(float dt, float tt, Renderer* renderer)
{

}

std::vector<GameObject*> Scene::getEntities()
{
	return entities;
}

void Scene::releaseEntities()
{
	std::vector<GameObject*>::iterator it;
	for (it = entities.begin(); it < entities.end(); it++) {
		delete *it;
	}

	entities.clear();
}

Octree* Scene::getEntityTree()
{
	return octree;
}

//Static Methods
void Scene::setActive(std::string name)
{
	if (activeScene != nullptr) {
		activeScene->releaseEntities();
	}
	
	activeScene = scenes[name];
	activeScene->init();
	activeScene->update(0, 0);
}

Scene* Scene::getActive()
{
	return activeScene;
}

void Scene::release()
{
	if (activeScene != nullptr) {
		activeScene->releaseEntities();
	}
	
	//Clean up scenes
	for (auto kv : Scene::scenes) {
		delete kv.second;
	}
}