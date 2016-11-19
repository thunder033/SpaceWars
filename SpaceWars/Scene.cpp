#include "Scene.h"

Scene* Scene::activeScene = nullptr;
std::map<std::string, Scene*> Scene::scenes;

Scene::Scene(std::string name)
{
	this->name = name;
	Scene::scenes[name] = this;
	octree = nullptr;
}


Scene::~Scene()
{
	releaseEntities();
}

void Scene::init()
{
	octree = std::unique_ptr<Octree>(new Octree(Vector3(-10.0f, -10.0f, -10.0f), Vector3(10.0f, 10.0f, 10.0), 1));
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

	if (entities.size() > 0) {

		std::vector<GameObject*>::iterator it;
		for (it = entities.begin(); it < entities.end(); it++) {
			delete *it;
		}

		entities.clear();
	}
}

Octree* Scene::getEntityTree()
{
	return octree.get();
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
	//Clean up scenes
	for (auto kv : Scene::scenes) {
		if (kv.second != nullptr) {
			delete kv.second;
		}
	}

	Scene::scenes.clear();
	activeScene = nullptr;
}