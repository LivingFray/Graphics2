#pragma once
#include "Light.h"
class SpotLight :
	public Light {
public:
	SpotLight();
	~SpotLight();
	void setScene(Scene * s);
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
	//Attenuation
	float constant;
	float linear;
	float quadratic;
};

