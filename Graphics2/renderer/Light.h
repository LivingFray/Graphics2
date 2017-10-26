#pragma once
#include "SceneObject.h"
#include "OpenGLSetup.h"
class Light :
	public SceneObject {
public:
	Light();
	virtual ~Light();
	glm::vec3 colour;
	GLuint depthMap;
};

