#include "DirectionalLight.h"
#include "Scene.h"


DirectionalLight::DirectionalLight() {
}


DirectionalLight::~DirectionalLight() {
}

void DirectionalLight::setScene(Scene* s) {
	Scene* old = this->getScene();
	SceneObject::setScene(s);
	//If the scene changed update the lists
	if (s != old) {
		if (old) {
			old->dirLight = nullptr;
		}
		s->dirLight = this;
	}
}