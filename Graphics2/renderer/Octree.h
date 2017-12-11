#pragma once
/*
Octree used to calculate collisions
*/
#include <vector>
#include <list>

#include "Shader.h"
#include "Camera.h"

#include "glm\common.hpp"

class Octree {
public:
	Octree();
	~Octree();
	void create(std::vector<unsigned short> &indices, std::vector<glm::vec3> &points, int maxDepth);
	void divide(std::vector<unsigned short> &indices, std::vector<glm::vec3> &points, float minX, float maxX, float minY, float maxY, float minZ, float maxZ, int depth);
	bool collides(Octree* other, glm::mat4 &trans, glm::mat4 &otherTrans, glm::mat4 &invTrans);
	//Debug drawing
//	void draw(glm::mat4 &trans, Camera* cam);
private:
	float minX, minY, minZ, maxX, maxY, maxZ;
	std::vector<glm::vec3> coords;
	std::vector<Octree*> children;
	bool containsTriangle(std::vector<glm::vec3> &tri, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
	void project(std::vector<glm::vec3> &points, glm::vec3 &axis, float &min, float &max);
	bool overlaps(Octree* other, glm::mat4 &trans, glm::mat4 &otherTrans);
	std::list<std::vector<glm::vec3>> tris;
	//Debug drawing
//	Shader shader;
//	GLuint vertexArray;
//	GLuint vertexBuffer;
};
