#include "OpenGLSetup.h"
#include <iostream>
//Flag that we could like to use the dedicated graphics card if available
//(Why my computer feels the need to use an integrated chip over
// a GTX 1060 by default I do not understand)
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

GLFWwindow* OpenGLSetup::window = nullptr;

void OpenGLSetup::init(int width, int height, const char* title,
		GLFWmonitor* monitor, GLFWwindow* share) {
	if (!glfwInit()) {
		std::cerr << "Could not initialise glfw" << std::endl;
		exit(1);
	}
	window = glfwCreateWindow(width, height, title, monitor, share);
	sharedInit();
}

void OpenGLSetup::initFullscreen(const char* title, GLFWmonitor* monitor) {
	if (!glfwInit()) {
		std::cerr << "Could not initialise glfw" << std::endl;
		exit(1);
	}
	if (monitor == nullptr) {
		monitor = glfwGetPrimaryMonitor();
	}
	//Google says this is how you enter borderless windowed mode
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	window = glfwCreateWindow(mode->width, mode->height, title, monitor, NULL);
	sharedInit();
}

void OpenGLSetup::destroy() {
	if (window) {
		glfwDestroyWindow(window);
	}
	glfwTerminate();
}

void OpenGLSetup::sharedInit() {
	if (!window) {
		std::cerr << "Could not create window" << std::endl;
		glfwTerminate();
		exit(1);
	}
	//New OpenGL only
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	//No ignoring the above please
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	//Don't know why I bother with this, I doubt this compiles on apple anyway
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Could not initialise GLEW" << std::endl;
		glfwTerminate();
		exit(1);
	}
	glCullFace(GL_BACK);
	//Useful debug info about the graphics card in use
	//(This is where I learnt I was using an integrated chip by default)
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl
		<< "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl
		<< "Vendor: " << glGetString(GL_VENDOR) << std::endl;
}
