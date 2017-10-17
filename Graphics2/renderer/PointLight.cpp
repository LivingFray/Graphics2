#include "PointLight.h"
#include "Scene.h"


PointLight::PointLight() {
}


PointLight::~PointLight() {
}

void PointLight::setScene(Scene* s) {
	Scene* old = this->getScene();
	SceneObject::setScene(s);
	//If the scene changed update the lists
	if (s != old) {
		if (old) {
			old->pointLights.erase(this);
		}
		s->pointLights.insert(this);
	}
}
