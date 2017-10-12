#include <iostream>
#include "renderer\OpenGLSetup.h"
#include "renderer\Scene.h"
#include "renderer\SceneObject.h"
#include "renderer\Cube.h"
#include "renderer\Mesh.h"
#include "renderer\Model.h"

#include "renderer\glm\gtc\quaternion.hpp"
#include "renderer\glm\gtc\matrix_transform.hpp"
#include "renderer\glm\gtc\matrix_inverse.hpp"

#define MOUSE_SPEED 0.005
#define CAMERA_SPEED 0.05f

Camera aCamera = Camera();

double pitch, yaw;

void windowResized(GLFWwindow* window, int width, int height) {
	aCamera.setWidth(width);
	aCamera.setHeight(height);
	glViewport(0, 0, width, height);
}

void updateCamera(double dt) {
	double mx, my;
	int w, h;
	glfwGetCursorPos(OpenGLSetup::window, &mx, &my);
	glfwGetWindowSize(OpenGLSetup::window, &w, &h);
	mx -= static_cast<double>(w) / 2.0;
	my -= static_cast<double>(h) / 2.0;
	glfwSetCursorPos(OpenGLSetup::window,
		static_cast<double>(w) / 2.0,
		static_cast<double>(h) / 2.0
	);

	yaw += mx * MOUSE_SPEED;
	pitch += my * MOUSE_SPEED;

	if (yaw > glm::two_pi<double>()) {
		yaw -= glm::two_pi<double>();
	}
	if (yaw < 0.0) {
		yaw += glm::two_pi<double>();
	}
	if (pitch > glm::half_pi<double>()) {
		pitch = glm::half_pi<double>();
	}
	if (pitch < -glm::half_pi<double>()) {
		pitch = -glm::half_pi<double>();
	}

	glm::vec3 pos = aCamera.getPosition();
	glm::vec3 front = glm::vec3(
		-sin(yaw) * cos(pitch),
		sin(pitch),
		cos(yaw) * cos(pitch)
	);
	glm::normalize(front);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);//aCamera.getUp();
	glm::vec3 right = glm::normalize(glm::cross(up, front));

	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_I)) {
		pos -= front * CAMERA_SPEED;
	}
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_K)) {
		pos += front * CAMERA_SPEED;
	}
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_J)) {
		pos -= right * CAMERA_SPEED;
	}
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_L)) {
		pos += right * CAMERA_SPEED;
	}
	aCamera.setRotation(glm::quat(glm::vec3(-pitch, -yaw, 0.0)));
	aCamera.setPosition(pos);
}

int main() {
	OpenGLSetup::init();
	Cube aCube;
	Cube anotherCube;
	Scene aScene;
	Model aModel;
	glfwSetWindowSizeCallback(OpenGLSetup::window, windowResized);
	int w, h;
	glfwGetWindowSize(OpenGLSetup::window, &w, &h);
	windowResized(OpenGLSetup::window, w, h);
	aScene.loadSkybox("assets/skybox/posX.png", "assets/skybox/negX.png",
		"assets/skybox/posY.png", "assets/skybox/negY.png",
		"assets/skybox/posZ.png", "assets/skybox/negZ.png");
	//aCamera.setParent(&aCube);
	aCamera.setParent(&aScene);
	aCube.setParent(&aScene);
	aCube.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	anotherCube.setRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.78f)));
	anotherCube.setParent(&aScene);
	anotherCube.setPosition(glm::vec3(0.0, 1.0, 0.0));
	anotherCube.setScale(glm::vec3(0.5, 0.5, 0.5));
	aCamera.setPosition(glm::vec3(0.0f, 0.0f, 6.0f));
	aModel.setParent(&aScene);
	//assert(aModel.loadModel("assets/testing/square.obj"));
	assert(aModel.loadModel("assets/nanosuit/nanosuit.obj"));
	aModel.setScale(glm::vec3(0.2, 0.2, 0.2));
	//aModel.setPosition(glm::vec3(0.0, 0.0, 3.0));
	aModel.setPosition(glm::vec3(0.0, -2.0, 3.0));
	//aModel.setRotation(glm::quat(glm::vec3(1.58, 0.0, 0.0)));
	//aCamera.setRotation(glm::quat(glm::vec3(0.0f, 0.2f, 0.0f)));
	glfwSetInputMode(OpenGLSetup::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	double time = glfwGetTime();
	double dt = 0.0;
	int frames = 0;
	double t = 0.0;
	float pos = 0.0f;
	float s = 0.5f;
	bool big = true;
	float cam = 0.0f;
	glfwSetCursorPos(OpenGLSetup::window,
		static_cast<double>(w) / 2.0,
		static_cast<double>(h) / 2.0
	);
	glfwSwapInterval(0);
	while (!glfwWindowShouldClose(OpenGLSetup::window) && !glfwGetKey(OpenGLSetup::window, GLFW_KEY_ESCAPE)) {
		//Update timers
		dt = glfwGetTime() - time;
		time = glfwGetTime();
		glfwPollEvents();
		//Update and draw
		pos += static_cast<float>(dt);
		if (big) {
			s += static_cast<float>(dt);
			big = s < 2.0f;
			s = big ? s : 2.0f;
		} else {
			s -= static_cast<float>(dt);
			big = s < 0.5f;
			s = big ? 0.5f : s;
		}
		cam += static_cast<float>(dt);
		if (cam > 6.28f) {
			cam -= 6.28f;
		}
		anotherCube.setScale(glm::vec3(s,s,s));
		//aCamera.setPosition(glm::vec3(0.0f, 0.0f, -pos));
		aModel.setRotation(glm::quat(glm::vec3(0.0f, cam, 0.0f)));
		updateCamera(dt);

		aCamera.render();
		//FPS counter
		frames++;
		t += dt;
		if (t > 1.0) {
			t -= 1.0;
			//TODO: Display on screen with text
			std::cout << "FPS: " << frames << std::endl;
			frames = 0;
		}
		glfwSwapBuffers(OpenGLSetup::window);
	}
	OpenGLSetup::destroy();
	return 0;
}