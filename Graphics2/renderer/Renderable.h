#pragma once
/*
Renderables are objects that have to be drawn.
Meshes are an obvious example of this.
*/
#include "SceneObject.h"
#include "Camera.h"
#include "Shader.h"
class Renderable :
	public SceneObject {
public:
	Renderable();
	virtual ~Renderable();
	virtual void render(Camera* cam) = 0;
protected:
	Shader shader;
private:
	void setScene(Scene* s);
};

