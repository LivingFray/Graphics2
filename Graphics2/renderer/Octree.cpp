#include "Octree.h"
#include "glm/glm.hpp"


Octree::Octree() {
	minX = INFINITY;
	minY = INFINITY;
	minZ = INFINITY;
	maxX = -INFINITY;
	maxY = -INFINITY;
	maxZ = -INFINITY;
//	shader = Shader("shaders/solid.vert", "shaders/solid.frag");
//	glGenVertexArrays(1, &vertexArray);
//	glGenBuffers(1, &vertexBuffer);
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
	coords.resize(8);
	coords[0] = glm::vec3(minX, minY, minZ);
	coords[1] = glm::vec3(minX, minY, maxZ);
	coords[2] = glm::vec3(minX, maxY, minZ);
	coords[3] = glm::vec3(minX, maxY, maxZ);
	coords[4] = glm::vec3(maxX, minY, minZ);
	coords[5] = glm::vec3(maxX, minY, maxZ);
	coords[6] = glm::vec3(maxX, maxY, minZ);
	coords[7] = glm::vec3(maxX, maxY, maxZ);

	//If depth = 0: Leaf node
	if (depth == 0) {
		for (unsigned int i = 0; i < indices.size() / 3; i+=3) {
			std::vector<glm::vec3> tri(3);
			tri[0] = points[indices[i + 0]];
			tri[1] = points[indices[i + 1]];
			tri[2] = points[indices[i + 2]];
			tris.push_back(tri);
		}
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
//	glBindVertexArray(vertexArray);
//	//Pass vertices
//	glEnableVertexAttribArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//	glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(glm::vec3), &coords[0], GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

bool Octree::collides(Octree* other, glm::mat4 &trans, glm::mat4 &otherTrans, glm::mat4 &invOtherTrans) {
	/* TRANSLATED PSEUDOCODE FROM LECTURES (so don't blame me if its wrong)
	If NOT overlap(this, other) return false
	else if leaf(this)
		if leaf(other)
			TRI COLLISION (maybe skip?), return true
		else
			for each child of other
				if this->collides(child)
					return true
	else
		for each child of this
			if child->collides(other)
				return true
	return false
	*/
	if (!overlaps(other, trans, otherTrans)) {
		return false;
	}
	if (children.size() == 0) {
		if (other->children.size() == 0) {
			glm::mat4 combTrans = trans * invOtherTrans;
			//For simplicity compare this objects octree with the other objects triangle
			for (std::vector<glm::vec3> tri : other->tris) {
				//Convert triangle to correct coordinate system
				std::vector<glm::vec3> transTri(3);
				transTri[0] = glm::vec3(combTrans * glm::vec4(tri[0], 1.0f));
				transTri[1] = glm::vec3(combTrans * glm::vec4(tri[1], 1.0f));
				transTri[2] = glm::vec3(combTrans * glm::vec4(tri[2], 1.0f));
				//Check for collision
				if (containsTriangle(transTri, minX, maxX, minY, maxY, minZ, maxZ)) {
					return true;
				}
			}
			return false;
		} else {
			for (Octree* o : other->children) {
				if (this->collides(o, trans, otherTrans, invOtherTrans)) {
					return true;
				}
			}
		}
	} else {
		for (Octree* o : this->children) {
			if (o->collides(other, trans, otherTrans, invOtherTrans)) {
				return true;
			}
		}
	}
	return false;
}

//void Octree::draw(glm::mat4 &trans, Camera* cam) {
//	glUseProgram(shader.getProgram());
//	glBindVertexArray(vertexArray);
//	glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "model"), 1, false, &(trans)[0][0]);
//	glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "view"), 1, false, &(cam->getView())[0][0]);
//	glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "projection"), 1, false, &(cam->getProjection())[0][0]);
//	glDrawArrays(GL_LINES, 0, coords.size());
//	glUseProgram(0);
//	for (Octree* child : children) {
//		child->draw(trans, cam);
//	}
//}

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

bool Octree::overlaps(Octree* other, glm::mat4 &trans, glm::mat4 &otherTrans) {
	glm::vec4 norms[] = {
		glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
	};
	//Project vertices onto each normal and test for intersection
	std::vector<glm::vec3> thisCoords;
	for (glm::vec3 c : coords) {
		thisCoords.push_back(glm::vec3(trans * glm::vec4(c, 1.0f)));
	}
	std::vector<glm::vec3> otherCoords;
	for (glm::vec3 c : other->coords) {
		otherCoords.push_back(glm::vec3(otherTrans * glm::vec4(c, 1.0f)));
	}
	for (int i = 0; i < 3; i++) {
		float min, max, min2, max2;
		glm::vec3 axis = glm::vec3(trans * norms[i]);
		project(thisCoords, axis, min, max);
		project(otherCoords, axis, min2, max2);
		//No overlap
		if (max <= min2 || min >= max2) {
			return false;
		}
		axis = glm::vec3(otherTrans * norms[i]);
		project(thisCoords, axis, min, max);
		project(otherCoords, axis, min2, max2);
		//No overlap
		if (max <= min2 || min >= max2) {
			return false;
		}
	}
	//Cross each edge of 1 with the edge of the other and test for line
	glm::vec3 edgesThis[] = {
		//--- to +--
		thisCoords[4 * 1 + 2 * 0 + 0] - thisCoords[4 * 0 + 2 * 0 + 0],
		//--- to -+-
		thisCoords[4 * 0 + 2 * 1 + 0] - thisCoords[4 * 0 + 2 * 0 + 0],
		//--- to --+
		thisCoords[4 * 0 + 2 * 0 + 1] - thisCoords[4 * 0 + 2 * 0 + 0],
	};
	glm::vec3 edgesOther[] = {
		//--- to +--
		otherCoords[4 * 1 + 2 * 0 + 0] - otherCoords[4 * 0 + 2 * 0 + 0],
		//--- to -+-
		otherCoords[4 * 0 + 2 * 1 + 0] - otherCoords[4 * 0 + 2 * 0 + 0],
		//--- to --+
		otherCoords[4 * 0 + 2 * 0 + 1] - otherCoords[4 * 0 + 2 * 0 + 0],
	};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			glm::vec3 axis = glm::cross(edgesThis[i], edgesOther[i]);
			float min, max, min2, max2;
			project(thisCoords, axis, min, max);
			project(otherCoords, axis, min2, max2);
			//No overlap
			if (max <= min2 || min >= max2) {
				return false;
			}
		}
	}
	return true;
}
