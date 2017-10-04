#include "SceneObject.h"



SceneObject::SceneObject() {
	//TODO: Initialise
	localMat = glm::mat4(1);
	globalMat = glm::mat4(1);
}

SceneObject::SceneObject(SceneObject & other) {
	//TODO: Copy
}


SceneObject::~SceneObject() {
	//TODO: Delete
}


bool SceneObject::setParent(SceneObject* obj) {
	//Ensure object exists, and doesn't create a loop
	if (obj == nullptr || obj == this || this->hasDescendent(obj)) {
		return false;
	}
	this->parent = obj;
	obj->children.push_back(this);
	return true;
}

bool SceneObject::hasChild(SceneObject* child) {
	for (SceneObject* s : this->children) {
		if (s == child) {
			return true;
		}
	}
	return false;
}

bool SceneObject::hasDescendent(SceneObject* child) {
	if (hasChild(child)) {
		return true;
	}
	for (SceneObject* s : this->children) {
		if (s->hasDescendent(child)) {
			return true;
		}
	}
	return false;
}

glm::mat4 SceneObject::getLocalMatrix() {
	return localMat;
}

glm::mat4 SceneObject::getGlobalMatrix() {
	return globalMat;
}

void SceneObject::setLocalMatrix(glm::mat4 m) {
	localMat = m;
	//Update global matrix for hierarchy
	updateMatrix();
}

void SceneObject::updateMatrix() {
	//Technically the scene can be transformed, and it lacks a parent
	if (this->parent) {
		//Apply local transformation, then parent, then parent's parent, etc
		this->globalMat = this->parent->getGlobalMatrix() * localMat;
	}
	//Update child matrices
	for (SceneObject* o : this->getChildren()) {
		o->updateMatrix();
	}
}

