#pragma once
#include "Light.h"
class PointLight :
	public Light {
public:
	PointLight();
	~PointLight();
	void setScene(Scene* s);
	glm::vec3 position;
	//Attenuation
	float constant;
	float linear;
	float quadratic;
};

