#include "SceneObject.h"
#include <iostream>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"
#include "Scene.h"

SceneObject::SceneObject() {
	localMat = glm::mat4(1);
	globalMat = glm::mat4(1);
	parent = nullptr;
	scene = nullptr;
	pos = glm::vec3(0.0);
	scale = glm::vec3(1.0);
	rot = glm::quat();
}

SceneObject::SceneObject(SceneObject& other) {
	//TODO: Copy
}


SceneObject::~SceneObject() {
	//TODO: Delete
}

bool SceneObject::setParent(Scene* obj) {
	if (!obj) {
		return false;
	}
	this->parent = (SceneObject*)obj;
	setScene(obj);
	return true;
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
		//Ensure correct scene
		if (dynamic_cast<Scene*>(obj)) {
			setScene(dynamic_cast<Scene*>(obj));
		} else {
			setScene(obj->scene);
		}
	} else {
		setScene(NULL);
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

void SceneObject::setLocalMatrix(glm::mat4 local) {
	localMat = local;
	updateMatrix();
	glm::vec3 skew;
	glm::vec4 pers;
	glm::decompose(localMat, scale, rot, pos, skew, pers);
}

glm::mat4 SceneObject::getLocalMatrix() {
	return localMat;
}

glm::mat4 SceneObject::getGlobalMatrix() {
	return globalMat;
}

void SceneObject::setPosition(glm::vec3 pos) {
	this->pos = pos;
	//Apply transformations in order: rotation, scale, translation
	localMat = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), scale) * glm::mat4_cast(rot);
	updateMatrix();
}

void SceneObject::setScale(glm::vec3 scale) {
	this->scale = scale;
	//Apply transformations in order: rotation, scale, translation
	localMat = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), scale) * glm::mat4_cast(rot);
	updateMatrix();
}

void SceneObject::setRotation(glm::quat rot) {
	this->rot = rot;
	//Apply transformations in order: rotation, scale, translation
	localMat = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), scale) * glm::mat4_cast(rot);
	updateMatrix();
}

glm::vec3 SceneObject::getPosition() {
	return pos;
}

glm::vec3 SceneObject::getScale() {
	return scale;
}

glm::quat SceneObject::getRotation() {
	return rot;
}

glm::vec3 SceneObject::getFront() {
	return glm::normalize(glm::mat3(localMat) * glm::vec3(0.0, 0.0, -1.0));
}

glm::vec3 SceneObject::getUp() {
	return glm::normalize(glm::mat3(localMat) * glm::vec3(0.0, 1.0, 0.0));
}

glm::vec3 SceneObject::getRight() {
	return glm::normalize(glm::mat3(localMat) * glm::vec3(1.0, 0.0, 0.0));
}

glm::vec3 SceneObject::getGlobalPosition() {
	return glm::vec3(globalMat[3]);
}

void SceneObject::updateMatrix() {
	//Technically the scene can be transformed, and it lacks a parent
	if (this->parent) {
		//Apply local transformation, then parent, then parent's parent, etc
		this->globalMat = this->parent->getGlobalMatrix() * localMat;
	} else {
		this->globalMat = localMat;
	}
	//Update child matrices
	for (SceneObject* o : this->getChildren()) {
		o->updateMatrix();
	}
}

void SceneObject::setScene(Scene* s) {
	this->scene = s;
	for (SceneObject* c : children) {
		c->setScene(s);
	}
}
