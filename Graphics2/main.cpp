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

#include "terrain\Planet.h"

#include "game\Game.h"

#define MOUSE_SPEED 0.005
#define CAMERA_SPEED 1.0f

Game* game;

double pitch, yaw;
int newCamW = 0;
int newCamH = 0;

void windowResized(GLFWwindow* window, int width, int height) {
	newCamW = width;
	newCamH = height;
	glViewport(0, 0, width, height);
}

void updateCamera(float dt, Camera &aCamera) {
	if (newCamW && aCamera.getPerspective()) {
		aCamera.setWidth(newCamW);
		aCamera.setHeight(newCamH);
		newCamW = 0;
		newCamH = 0;
	}
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
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right = glm::normalize(glm::cross(up, front));

	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_I)) {
		pos -= front * CAMERA_SPEED * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_K)) {
		pos += front * CAMERA_SPEED * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_J)) {
		pos -= right * CAMERA_SPEED * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_L)) {
		pos += right * CAMERA_SPEED * dt;
	}
	aCamera.setRotation(glm::quat(glm::vec3(-pitch, -yaw, 0.0)));
	aCamera.setPosition(pos);
}

void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	game->keyEvent(window, key, scancode, action, mods);
}

int main() {
	OpenGLSetup::init();
	game = new Game();

	glfwSetWindowSizeCallback(OpenGLSetup::window, windowResized);
	glfwSetKeyCallback(OpenGLSetup::window, keyEvent);
	int w, h;
	glfwGetWindowSize(OpenGLSetup::window, &w, &h);
	windowResized(OpenGLSetup::window, w, h);

	glfwSetInputMode(OpenGLSetup::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	double time = glfwGetTime();
	double dt = 0.0;
	int frames = 0;
	double t = 0.0;
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
		//Update game
		game->update(dt);
		//Draw game
		game->draw();
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