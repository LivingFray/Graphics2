#include <iostream>
#include "renderer\OpenGLSetup.h"
#include "renderer\Scene.h"
#include "renderer\SceneObject.h"


int main() {
	OpenGLSetup::init();
	while (!glfwWindowShouldClose(OpenGLSetup::window) && !glfwGetKey(OpenGLSetup::window, GLFW_KEY_ESCAPE)) {
		glfwPollEvents();
		//Draw and update
	}
	OpenGLSetup::destroy();
	return 0;
}