#include "SpotLight.h"
#include "Scene.h"


SpotLight::SpotLight() {
}


SpotLight::~SpotLight() {
}

void SpotLight::setScene(Scene* s) {
	Scene* old = this->getScene();
	SceneObject::setScene(s);
	//If the scene changed update the lists
	if (s != old) {
		if (old) {
			old->spotLights.erase(this);
		}
		s->spotLights.insert(this);
	}
}
