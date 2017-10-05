#pragma once
/*
Renderables are objects that have to be drawn.
Meshes are an obvious example of this.
*/
#include "SceneObject.h"
#include "Camera.h"
class Renderable :
	public SceneObject {
public:
	Renderable();
	virtual ~Renderable();
	void render(const Camera* cam);
};

