#pragma once
/*
Sets up all the necessary OpenGL things
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32.lib")

class OpenGLSetup {
public:
	//Creates a window
	static void init(int width = 1280, int height = 720,
		const char* title = "OpenGL 3.3",
		GLFWmonitor* monitor = NULL,
		GLFWwindow* share = NULL);
	//Creates a fullscreen instance
	static void initFullscreen(const char* title = "OpenGL game",
		GLFWmonitor* monitor = NULL);
	//Shuts everything down
	static void destroy();
	//Loads an image from file
	static GLuint loadImage(std::string filename);
	static GLFWwindow* window;
private:
	//Does most of the heavy lifting when initialising
	static void sharedInit();
};
