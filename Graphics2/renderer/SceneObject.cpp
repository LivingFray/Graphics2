#include "SceneObject.h"
#include <iostream>
#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"

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
	if (obj == this || this->hasDescendent(obj)) {
		std::cerr << "Circular reference in object hierarchy" << std::endl;
		throw;
	}
	this->parent = obj;
	if (obj) {
		obj->children.push_back(this);
	}
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

void SceneObject::setPosition(glm::vec3 pos) {
	this->pos = pos;
	updateMatrix();
}

void SceneObject::setScale(glm::vec3 scale) {
	this->scale = scale;
	updateMatrix();
}

void SceneObject::setRotation(glm::quat rot) {
	this->rot = rot;
	updateMatrix();
}

void SceneObject::updateMatrix() {
	//Apply transformations in order: rotation, scale, translation
	localMat = glm::translate(pos) * glm::scale(scale) * glm::toMat4(rot);
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
