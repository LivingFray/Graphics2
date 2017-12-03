#pragma once
#include "Renderable.h"
#include "Mesh.h"
class Portal :
	public Renderable {
public:
	Portal();
	~Portal();
	void initPortalMap();
	void render(Camera* cam, GLuint depthMap, glm::mat4& LSM);
	void renderShadow(GLuint program) {};
	SceneObject* exitPortal;
	Camera* renderView;
	Model* portalSurface;
	float portalScale;
private:
	GLuint fbo;
	GLuint rbo;
	GLuint renderTo;
	Shader portal;
};

