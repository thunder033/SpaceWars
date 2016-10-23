#pragma once

#include <string>
#include <vector>
#include "Entity.h"
#include "Renderer.h"

class Scene
{
	static Scene* activeScene;
	static std::map<std::string, Scene*> scenes;

protected:
	std::string name;
	std::vector<Entity*> entities;

public:
	Scene(std::string name);
	virtual ~Scene();

	virtual void init();
	virtual void update(float dt, float tt);
	virtual void draw(float dt, float tt, Renderer* renderer);
	std::vector<Entity*> getEntities();
	void releaseEntities();

	static void setActive(std::string name);
	static Scene* getActive();
	static void release();
};

