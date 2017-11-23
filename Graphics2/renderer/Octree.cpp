#include "Octree.h"
#include "glm/glm.hpp"


Octree::Octree() {
	minX = INFINITY;
	minY = INFINITY;
	minZ = INFINITY;
	maxX = -INFINITY;
	maxY = -INFINITY;
	maxZ = -INFINITY;
}


Octree::~Octree() {
	for (Octree* child : children) {
		if (child) {
			delete child;
		}
	}
}

void Octree::create(std::vector<unsigned short> &indices, std::vector<glm::vec3> &points, int maxDepth) {
	//Go through each point to determine boundary
	for (glm::vec3 point : points) {
		if (point.x < minX) {
			minX = point.x;
		}
		if (point.x > maxX) {
			maxX = point.x;
		}
		if (point.y < minY) {
			minY = point.y;
		}
		if (point.y > maxY) {
			maxY = point.y;
		}
		if (point.z < minZ) {
			minZ = point.z;
		}
		if (point.z > maxZ) {
			maxZ = point.z;
		}
	}
	//Subdivide
	divide(indices, points, minX, maxX, minY, maxY, minZ, maxZ, maxDepth);
}

void Octree::divide(std::vector<unsigned short> &indices, std::vector<glm::vec3> &points, float minX, float maxX, float minY, float maxY, float minZ, float maxZ, int depth) {
	this->minX = minX;
	this->maxX = maxX;
	this->minY = minY;
	this->maxY = maxY;
	this->minZ = minZ;
	this->maxZ = maxZ;
	//If depth = 0: Leaf node
	if (depth == 0) {
		return;
	}
	//Decrease depth
	depth--;
	//Split points into 8 vectors
	float midX = (minX + maxX) / 2;
	float midY = (minY + maxY) / 2;
	float midZ = (minZ + maxZ) / 2;
	std::vector<unsigned short> split[8];
	//Try linked list after?
	//---,--+,-+-,-++,+--,+-+,++-,+++
	for (unsigned int i = 0; i < indices.size(); i+=3) {
		std::vector<glm::vec3> p;
		p.resize(3);
		p[0] = points[indices[i + 0]];
		p[1] = points[indices[i + 1]];
		p[2] = points[indices[i + 2]];
		for (int x = 0; x < 2; x++) {
			float xMin = x == 0 ? minX : midX;
			float xMax = x == 0 ? midX : maxX;
			for (int y = 0; y < 2; y++) {
				float yMin = y == 0 ? minY : midY;
				float yMax = y == 0 ? midY : maxY;
				for (int z = 0; z < 2; z++) {
					float zMin = z == 0 ? minZ : midZ;
					float zMax = z == 0 ? midZ : maxZ;
					if (containsTriangle(p, xMin, xMax, yMin, yMax, zMin, zMax)) {
						split[x * 4 + y * 2 + z].push_back(indices[i + 0]);
						split[x * 4 + y * 2 + z].push_back(indices[i + 1]);
						split[x * 4 + y * 2 + z].push_back(indices[i + 2]);
					}
				}
			}
		}
	}
	//Create octree children with these points
	for (int x = 0; x < 2; x++) {
		float xMin = x == 0 ? minX : midX;
		float xMax = x == 0 ? midX : maxX;
		for (int y = 0; y < 2; y++) {
			float yMin = y == 0 ? minY : midY;
			float yMax = y == 0 ? midY : maxY;
			for (int z = 0; z < 2; z++) {
				float zMin = z == 0 ? minZ : midZ;
				float zMax = z == 0 ? midZ : maxZ;
				if (split[x * 4 + y * 2 + z].size() > 0) {
					Octree* child = new Octree();
					child->divide(split[x * 4 + y * 2 + z], points, xMin, xMax, yMin, yMax, zMin, zMax, depth);
					children.push_back(child);
				}
			}
		}
	}
}

bool Octree::collides(Octree* other) {
	return false;
}

bool Octree::containsTriangle(std::vector<glm::vec3> &tri, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {
	//Test AABB normals first
	glm::vec3 norms[3] = {
		glm::vec3(1,0,0), //X
		glm::vec3(0,1,0), //Y
		glm::vec3(0,0,1), //Z
	};
	float min, max;
	std::vector<glm::vec3> aabb;
	aabb.resize(2);
	aabb[0] = glm::vec3(xMin, yMin, zMin);
	aabb[1] = glm::vec3(xMax, yMax, zMax);
	//Project onto each cardinal direction and check for overlaps
	for (int i = 0; i < 3; i++) {
		project(tri, norms[i], min, max);
		if (max < aabb[0][i] || min > aabb[1][i]) {
			return false;
		}
	}
	//Check triangle normal
	glm::vec3 triNorm = glm::cross(tri[2] - tri[1], tri[2] - tri[0]);
	float triOffset = glm::dot(triNorm, tri[0]);
	project(aabb, triNorm, min, max);
	if (max < triOffset || min > triOffset) {
		return false;
	}
	//Check cross products
	glm::vec3 triEdges[3] = {
		tri[0] - tri[1],
		tri[1] - tri[2],
		tri[2] - tri[0]
	};

	float min2, max2;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			glm::vec3 axis = glm::cross(triEdges[i], norms[j]);
			project(aabb, axis, min, max);
			project(tri, axis, min2, max2);
			if (max <= min2 || min >= max2) {
				return false;
			}
		}
	}
	//Every projection failed, so must intersect
	return true;
}

void Octree::project(std::vector<glm::vec3> &points, glm::vec3 &axis, float &min, float &max) {
	min = INFINITY;
	max = -INFINITY;
	for(glm::vec3 p: points) {
		float val = glm::dot(axis, p);
		if (val < min) { min = val; }
		if (val > max) { max = val; }
	}
}
