#pragma once
#include "SceneObject.h"
class Light :
	public SceneObject {
public:
	Light();
	virtual ~Light();
	glm::vec3 diffuse;
	glm::vec3 specular;
};

