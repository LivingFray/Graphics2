#pragma once
/*
Simplified Octree that is used for collisions, and doesn't store vertex information
*/
#include <vector>

#include "glm\common.hpp"

class Octree {
public:
	Octree();
	~Octree();
	void create(std::vector<unsigned short> &indices, std::vector<glm::vec3> &points, int maxDepth);
	void divide(std::vector<unsigned short> &indices, std::vector<glm::vec3> &points, float minX, float maxX, float minY, float maxY, float minZ, float maxZ, int depth);
	bool collides(Octree* other);
private:
	float minX, minY, minZ, maxX, maxY, maxZ;
	std::vector<Octree*> children;
	bool containsTriangle(std::vector<glm::vec3> &tri, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
	void project(std::vector<glm::vec3> &points, glm::vec3 &axis, float &min, float &max);
};
