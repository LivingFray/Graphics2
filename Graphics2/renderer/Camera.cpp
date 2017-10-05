#include "Camera.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"


Camera::Camera() {
	fov = glm::pi<float>() / 3.0f;
	perspective = true;
	near = 0.1f;
	far = 100.0f;
	width = 1.0f;
	height = 1.0f;
	proj = glm::perspective(fov, width / height, near, far);
}


Camera::~Camera() {
}

void Camera::setFOV(GLfloat fov) {
	this->fov = fov;
}

void Camera::setPerspective(bool perspective) {
	this->perspective = perspective;
}

void Camera::setNear(GLfloat near) {
	this->near = near;
}

void Camera::setFar(GLfloat near) {
	this->far = far;
}

void Camera::setWidth(GLfloat width) {
	this->width = width;
}

void Camera::setHeight(GLfloat height) {
	this->height = height;
}

glm::mat4 Camera::getProjection() {
	if (updateFlag) {
		if (perspective) {
			proj = glm::perspective(fov, width / height, near, far);
		} else {
			proj = glm::ortho(-width / 2, -height / 2, width / 2, height / 2);
		}
		updateFlag = false;
	}
	return proj;
}

glm::mat4 Camera::getView() const {
	glm::mat4 view = glm::translate(glm::mat4(1.0f), -getPosition());
	view = glm::mat4_cast(getRotation()) * view;
	return view;
}
