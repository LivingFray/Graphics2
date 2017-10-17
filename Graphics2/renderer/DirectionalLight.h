#pragma once
#include "Light.h"
class DirectionalLight :
	public Light {
public:
	DirectionalLight();
	~DirectionalLight();
	void setScene(Scene* s);
	glm::vec3 direction;
};

