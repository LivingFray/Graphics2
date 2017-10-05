#include "Camera.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Scene.h"
#include <iostream>

Camera::Camera() {
	fov = glm::pi<float>() / 3.0f;
	perspective = true;
	near = 0.1f;
	far = 100.0f;
	int w, h;
	glfwGetWindowSize(OpenGLSetup::window, &w, &h);
	width = static_cast<float>(w);
	height = static_cast<float>(h);
	proj = glm::perspective(fov, width / height, near, far);
	clearOnDraw = true;
	updateFlag = false;
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

void Camera::setFar(GLfloat far) {
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

void Camera::render() {
	if (!getScene()) {
		std::cerr << "No scene associated with camera" << std::endl;
		return;
	}
	if (clearOnDraw) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//TODO: space partitioning and culling and such
	set<Renderable*> renderables = getScene()->getRenderables();
	for (Renderable* r : renderables) {
		r->render(this);
	}
}
