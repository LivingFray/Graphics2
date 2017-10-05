#include <iostream>
#include "renderer\OpenGLSetup.h"
#include "renderer\Scene.h"
#include "renderer\SceneObject.h"
#include "renderer\Cube.h"

int main() {
	OpenGLSetup::init();
	Cube aCube, anotherCube;
	Scene aScene;
	Camera aCamera;
	aCamera.setParent(&aScene);
	aCube.setParent(&aScene);
	aCube.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	anotherCube.setRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.78f)));
	anotherCube.setParent(&aCube);
	anotherCube.setPosition(glm::vec3(0.0, 1.0, 0.0));
	anotherCube.setScale(glm::vec3(0.5, 0.5, 0.5));
	aCamera.setPosition(glm::vec3(0.0f, 0.0f, 6.0f));
	//aCamera.setRotation(glm::quat(glm::vec3(0.0f, 0.2f, 0.0f)));
	double time = glfwGetTime();
	double dt = 0;
	int frames = 0;
	double t = 0;
	float pos = 0;
	float s = 0.5;
	bool big = true;
	float cam = 0.0f;
	glfwSwapInterval(0);
	while (!glfwWindowShouldClose(OpenGLSetup::window) && !glfwGetKey(OpenGLSetup::window, GLFW_KEY_ESCAPE)) {
		//Update timers
		dt = glfwGetTime() - time;
		time = glfwGetTime();
		glfwPollEvents();
		//Draw and update
		aCamera.render();
		pos += dt;
		if (big) {
			s += dt;
			big = s < 2.0f;
			s = big ? s : 2.0f;
		} else {
			s -= dt;
			big = s < 0.5f;
			s = big ? 0.5f : s;
		}
		cam += dt;
		if (cam > 6.28) {
			cam -= 6.28;
		}
		anotherCube.setScale(glm::vec3(s,s,s));
		aCube.setPosition(glm::vec3(0.0f, 0.0f, -pos));
		//aCamera.setRotation(glm::quat(glm::vec3(0.0f, cam, 0.0f)));
		//FPS counter
		frames++;
		t += dt;
		if (t > 1) {
			t -= 1;
			//TODO: Display on screen with text
			std::cout << "FPS: " << frames << std::endl;
			frames = 0;
		}
		glfwSwapBuffers(OpenGLSetup::window);
	}
	OpenGLSetup::destroy();
	return 0;
}