#pragma once
#include <vector>
#include "..\renderer\Mesh.h"
class Planet {
public:
	Planet();
	~Planet();
	void generateTerrain();
	Mesh planet;
	Mesh water;
	//TODO: Split "land" into different materials (sand, grass, rock, ice)
	Mesh land;
	unsigned int seed;
private:
	void setNode(float value, unsigned int face, unsigned int x, unsigned int y);
	float getNode(int face, int x, int y);
	glm::vec3 inline getVertex(glm::mat4 trans, int x, int y, int face);
	glm::vec3 getVertex(glm::mat4 trans, int x, int y, int face, float height);
	void addTriangle(int f, unsigned int xs[], unsigned int ys[]);
	std::vector<std::vector<std::vector<float>>> faces;
	glm::mat4 faceTrans[6];
	std::vector<glm::vec3> vert;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> norm;
	std::vector<glm::vec3> tan;
	std::vector<glm::vec3> bitan;

	std::vector<glm::vec3> seaVert;
	std::vector<glm::vec2> seaUv;
	std::vector<glm::vec3> seaNorm;
	std::vector<glm::vec3> seaTan;
	std::vector<glm::vec3> seaBitan;
};

