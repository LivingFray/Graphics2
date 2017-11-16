#include "Renderable.h"
#include "Scene.h"


Renderable::Renderable() {
}


Renderable::~Renderable() {
}

void Renderable::setScene(Scene* s) {
	Scene* old = this->getScene();
	SceneObject::setScene(s);
	//If the scene changed update the lists
	if (s != old) {
		if (old) {
			old->renderables.erase(this);
		}
		if (s) {
			s->renderables.insert(this);
		}
	}
}
