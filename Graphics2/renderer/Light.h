#pragma once
#include "SceneObject.h"
class Light :
	public SceneObject {
public:
	Light();
	virtual ~Light();
	glm::vec3 colour;
};

