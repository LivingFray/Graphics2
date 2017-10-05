#include "Shader.h"
#include <iostream>
#include <fstream>
#include <vector>

Shader::Shader(std::string vert, std::string frag) {
	std::string vertString = readFile(vert);
	std::string fragString = readFile(frag);
	const char* vertChars = vertString.c_str();
	const char* fragChars = fragString.c_str();
	if (*vertChars && *fragChars) {
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		//Compile vertex shader
		glShaderSource(vertex, 1, &vertChars, NULL);
		glCompileShader(vertex);
		//Check for errors
		GLint result = GL_FALSE;
		int logLength;
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
		glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &logLength);
		if (result != GL_TRUE || logLength > 0) {
			std::vector<char> errorMessage(logLength + 1);
			glGetShaderInfoLog(vertex, logLength, NULL, &errorMessage[0]);
			printf("ERROR: %s\n", &errorMessage[0]);
		}
		//Compile fragment shader
		glShaderSource(fragment, 1, &fragChars, NULL);
		glCompileShader(fragment);
		//Check for errors
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
		glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &logLength);
		if (result != GL_TRUE || logLength > 0) {
			std::vector<char> errorMessage(logLength + 1);
			glGetShaderInfoLog(fragment, logLength, NULL, &errorMessage[0]);
			printf("ERROR: %s\n", &errorMessage[0]);
		}
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);
		glGetProgramiv(id, GL_LINK_STATUS, &result);
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
		if (result != GL_TRUE || logLength > 0) {
			std::vector<char> errorMessage(logLength + 1);
			glGetProgramInfoLog(id, logLength, NULL, &errorMessage[0]);
			printf("ERROR: %s\n", &errorMessage[0]);
		}
		glDetachShader(id, vertex);
		glDetachShader(id, fragment);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	std::cerr << "Couldn't loader shaders" << std::endl;
}

std::string Shader::readFile(std::string filename) {
	std::ifstream inStream(filename, std::ios::in);
	if (!inStream.is_open()) {
		return "";
	}
	std::string file;
	std::string line;
	while (std::getline(inStream, line))
		file += "\n" + line;
	inStream.close();
	return file;
}

Shader::~Shader() {
	glDeleteProgram(id);
}
