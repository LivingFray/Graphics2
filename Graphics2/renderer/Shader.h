#pragma once
/*
A wrapper for the GLSL shaders
*/
#include <string>
#include <map>
#include "OpenGLSetup.h"
class Shader {
public:
	Shader();
	Shader(std::string vert, std::string frag);
	~Shader();
	GLuint getProgram() { return id; };
private:
	std::string readFile(std::string filename);
	GLuint id;
	static std::map<std::string, GLuint> shaders;
};

