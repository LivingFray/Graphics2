#include "SceneObject.h"



SceneObject::SceneObject() {
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

SceneObject * SceneObject::getParent() {
	return parent;
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
