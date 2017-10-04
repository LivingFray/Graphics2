#pragma once
/*
A scene is everything that is rendered, multiple scenes can be rendered at once
by combining multiple cameras (example use for this is drawing planets with one
scene and local objects like spaceships with another)
*/
#include <vector>
#include "SceneObject.h"
#include "Renderable.h"

using std::vector;

class Scene: public SceneObject{
public:
	Scene();
	~Scene();

private:
	//Special rendering list
	vector<Renderable*> renderable;
	//Separate variable for skybox?
	//Make skybox related to camera?
};

