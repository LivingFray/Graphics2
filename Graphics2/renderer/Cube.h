#pragma once
/*
A simple cube to test rendering
*/
#include "Renderable.h"
class Cube :
	public Renderable {
public:
	Cube();
	~Cube();
	void render(Camera* cam, GLuint depthMap, glm::mat4& LSM);
	void renderShadow(GLuint p) {};
private:
	std::vector<GLfloat> vertices;
	GLuint vertexArray;
	GLuint vertexBuffer;
};

