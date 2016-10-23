#pragma once

#include <string>
#include <vector>
#include "Entity.h"
#include "Renderer.h"

class Scene
{
	//Static scene management
	static Scene* activeScene;
	static std::map<std::string, Scene*> scenes;

protected:
	std::string name; //Name of the scene
	std::vector<Entity*> entities; //The collection of entities associated with this scene

public:
	Scene(std::string name);
	virtual ~Scene();

	//setup for the scene to be run - this is called when scene becomes active
	virtual void init(); 
	//advance simulation in the scene
	virtual void update(float deltaTime, float totalTime);
	//draw commands for the scene
	virtual void draw(float deltaTime, float totalTime, Renderer* renderer);
	//returns the scene's collection of entities
	std::vector<Entity*> getEntities(); 
	//Delete all entities for the scene and clear the collection - called when the scene is deactivated
	void releaseEntities();

	//Static scene state managment - could be moved to a Stage class or something
	static void setActive(std::string name);
	static Scene* getActive();
	static void release();
};

