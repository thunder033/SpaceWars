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
}

void Scene::update(float dt, float tt)
{

};

void Scene::draw(float dt, float tt, Renderer* renderer)
{

}

std::vector<Entity*> Scene::getEntities()
{
	return entities;
}

void Scene::releaseEntities()
{
	std::vector<Entity*>::iterator it;
	for (it = entities.begin(); it < entities.end(); it++) {
		delete *it;
	}

	entities.clear();
}

//Static Methods
void Scene::setActive(std::string name)
{
	if (activeScene != nullptr) {
		activeScene->releaseEntities();
	}
	
	activeScene = scenes[name];
	activeScene->init();
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