#include "Camera.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
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
	if (width < 1) { width = 1; };
	if (height < 1) { height = 1; };
	proj = glm::perspective(fov, width / height, near, far);
	clearOnDraw = true;
	updateFlag = false;
	shadow = Shader("shaders/shadow.vert", "shaders/shadow.frag");
	glViewport(0, 0, shadowMapSize, shadowMapSize);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
	initShadowMap();
}


Camera::~Camera() {
}

void Camera::initShadowMap() {
	//Generate buffers
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &depthMap);
	//Setup depth map texture
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//Create framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Camera::setFOV(GLfloat fov) {
	this->fov = fov;
	updateFlag = true;
}

void Camera::setPerspective(bool perspective) {
	this->perspective = perspective;
	updateFlag = true;
}

void Camera::setNear(GLfloat near) {
	this->near = near;
	updateFlag = true;
}

void Camera::setFar(GLfloat far) {
	this->far = far;
	updateFlag = true;
}

void Camera::setWidth(GLfloat width) {
	this->width = width;
	updateFlag = true;
}

void Camera::setHeight(GLfloat height) {
	this->height = height;
	updateFlag = true;
}

glm::mat4 Camera::getProjection() {
	if (updateFlag) {
		if (perspective) {
			proj = glm::perspective(fov, width / height, near, far);
		} else {
			proj = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, near, far);
		}
		updateFlag = false;
	}
	return proj;
}

glm::mat4 Camera::getView() {
	return glm::affineInverse(getGlobalMatrix());
}

void Camera::render() {
	if (!getScene()) {
		std::cerr << "No scene associated with camera" << std::endl;
		return;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//TODO: space partitioning and culling and such
	getScene()->updateLights();
	set<Renderable*> renderables = getScene()->getRenderables();
	//Directional lighting shadows
	DirectionalLight* d = getScene()->getDirectionalLight();
	//This needs changing for larger scenes
	//Calculate directional light projection
	float near_plane = 1.0f, far_plane = 100.0f;
	float orthosize = 50.0f;
	glm::mat4 lightProjection = glm::ortho(-orthosize, orthosize, -orthosize, orthosize, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(d->direction * -50.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 LSM = lightProjection * lightView;
	//Pass matrix to shader
	glUseProgram(shadow.getProgram());
	glUniformMatrix4fv(glGetUniformLocation(shadow.getProgram(), "lightSpaceMatrix"), 1, false, &LSM[0][0]);
	//Enable correct buffer
	glViewport(0, 0, shadowMapSize, shadowMapSize);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	//Draw shadows
	for (Renderable* r : renderables) {
		r->renderShadow(shadow.getProgram());
	}
	glCullFace(GL_BACK);
	//Reset buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Fix viewport
	int w, h;
	glfwGetWindowSize(OpenGLSetup::window, &w, &h);
	glViewport(0, 0, w, h);
	//TODO: Experiment with multiple maps vs re rendering scene
	if (clearOnDraw) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	//Render normally
	for (Renderable* r : renderables) {
		r->render(this, depthMap, LSM);
	}
	//Lastly, render the skybox
	this->getScene()->renderSkybox(this);
}
