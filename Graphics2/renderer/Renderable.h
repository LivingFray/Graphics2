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
	virtual void render(Camera* cam, GLuint depthMap, glm::mat4& LSM) = 0;
	virtual void renderShadow(GLuint p) = 0;
protected:
	Shader shader;
private:
	void setScene(Scene* s);
};

