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

#define MOUSE_SPEED 0.005
#define CAMERA_SPEED 0.00005f

double pitch, yaw;
int newCamW = 0;
int newCamH = 0;

void windowResized(GLFWwindow* window, int width, int height) {
	newCamW = width;
	newCamH = height;
	glViewport(0, 0, width, height);
}

void updateCamera(double dt, Camera &aCamera) {
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
	Camera aCamera;
	Scene aScene;
	DirectionalLight sunLight;

	Planet planet;
	glfwSetWindowSizeCallback(OpenGLSetup::window, windowResized);
	int w, h;
	glfwGetWindowSize(OpenGLSetup::window, &w, &h);
	windowResized(OpenGLSetup::window, w, h);
	////Skybox is very large, and causes massive slowdown in load times
	//aScene.loadSkybox("assets/skybox/posX.png", "assets/skybox/negX.png",
	//	"assets/skybox/posY.png", "assets/skybox/negY.png",
	//	"assets/skybox/posZ.png", "assets/skybox/negZ.png");
	//In case of messed up skybox, use this for debug
	aScene.loadSkybox("assets/skybox/right.png", "assets/skybox/left.png",
		"assets/skybox/top.png", "assets/skybox/bottom.png",
		"assets/skybox/back.png", "assets/skybox/front.png");
	aCamera.setParent(&aScene);
	aCamera.setPosition(glm::vec3(0.0f, 0.0f, 2.0f));
	aCamera.setFar(400.0f);
	planet.generateTerrain();
	planet.planet.setDiffuse(OpenGLSetup::loadImage("assets/testing/starfield.png"));
	planet.planet.setSpecular(0);
	planet.planet.setNormal(0);
	planet.planet.setEmission(0);
	planet.planet.setParent(&aScene);
	Cube forScale;
	forScale.setPosition(glm::vec3(0.0f, 2.0f, 0.0f));
	forScale.setParent(&aScene);
	//////Lighting
	sunLight.direction = glm::normalize(glm::vec3(0.0f, -1.0f, 2.0f));
	sunLight.colour = glm::vec3(1.0f, 1.0f, 1.0f) * 0.4f;
	sunLight.setParent(&aScene);
	aScene.ambientLight = glm::vec3(0.2, 0.2, 0.2) * 5.0f;
	//aScene.ambientLight = glm::vec3(0.0f, 0.0f, 0.0f);

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
		//Update and draw
		//torch.setPosition(aCamera.getPosition());
		//torch.direction = aCamera.getFront();
		//redLight.setPosition(aCamera.getPosition());
		updateCamera(dt, aCamera);
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