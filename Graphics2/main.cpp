#include <iostream>
#include "renderer\OpenGLSetup.h"
#include "renderer\Scene.h"
#include "renderer\SceneObject.h"
#include "renderer\Cube.h"
#include "renderer\Mesh.h"
#include "renderer\Model.h"

int main() {
	OpenGLSetup::init();
	Cube aCube, anotherCube;
	Scene aScene;
	Camera aCamera;
	Model aModel;
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
	double time = glfwGetTime();
	double dt = 0.0;
	int frames = 0;
	double t = 0.0;
	float pos = 0.0f;
	float s = 0.5f;
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
		aCube.setPosition(glm::vec3(0.0f, 0.0f, -pos));
		aModel.setRotation(glm::quat(glm::vec3(0.0f, cam, 0.0f)));
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