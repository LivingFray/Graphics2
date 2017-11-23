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
//	assert(false); //PROPAGATE UPDATED CAMERA DIMENSIONS
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