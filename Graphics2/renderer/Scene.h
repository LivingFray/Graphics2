#pragma once
/*
A scene is everything that is rendered, multiple scenes can be rendered at once
by combining multiple cameras (example use for this is drawing planets with one
scene and local objects like spaceships with another)
*/
#include <set>
#include "SceneObject.h"
#include "Renderable.h"

using std::set;

class Scene: public SceneObject{
public:
	Scene();
	~Scene();
	const set<Renderable*>& getRenderables() { return renderables; };
private:
	//Special rendering list
	set<Renderable*> renderables;
	friend Renderable;
	//list<Camera*> cameras;
	//Separate variable for skybox?
	//Make skybox related to camera?
};

