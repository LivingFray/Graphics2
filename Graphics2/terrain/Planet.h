#pragma once
#include <vector>
#include "..\renderer\Mesh.h"
class Planet {
public:
	Planet();
	~Planet();
	void generateTerrain();
	Mesh planet;
private:
	void setNode(float value, unsigned int face, unsigned int x, unsigned int y);
	std::vector<std::vector<std::vector<float>>> faces;
};

