#pragma once
#include <vector>
#include "..\renderer\Mesh.h"
#include "..\renderer\Scene.h"

class Planet {
public:
	Planet();
	~Planet();
	void generateTerrain();

	void TEST(Scene* p) {
		for (int l = 0; l < 5; l++) {
			for (int f = 0; f < 6; f++) {
				for (int gx = 0; gx < LODS[l][f].size(); gx++) {
					for (int gy = 0; gy < LODS[l][f][gx].size(); gy++) {
						PlanetMeshes meshes = LODS[l][f][gx][gy];
						if (meshes.sea) {
							meshes.sea->setParent(p);
							meshes.sea->setPosition(glm::vec3(l * 10.0f, 0.0f, 0.0f));
							meshes.sea->setDiffuse(OpenGLSetup::loadImage("assets/testing/water.png"));
						}
						if (meshes.grass) {
							meshes.grass->setParent(p);
							meshes.grass->setPosition(glm::vec3(l * 10.0f, 0.0f, 0.0f));
							meshes.grass->setDiffuse(OpenGLSetup::loadImage("assets/testing/grass.png"));
						}
						if (meshes.rock) {
							meshes.rock->setParent(p);
							meshes.rock->setPosition(glm::vec3(l * 10.0f, 0.0f, 0.0f));
							meshes.rock->setDiffuse(OpenGLSetup::loadImage("assets/testing/rock.png"));
						}
					}
				}
			}
		}
	};

	unsigned int seed;
private:
	struct PlanetMeshes {
		Mesh* sea;
		Mesh* grass;
		Mesh* rock;
	};
	//Terrain generation helper methods
	void inline diamondSquare();
	void inline createTransformations();
	void generateGrid(int l, int face, int minX, int minY, int maxX, int maxY);
	void inline makeMeshes(int l, int face, int gridX, int gridY);

	void setNode(float value, unsigned int face, unsigned int x, unsigned int y);
	float getNode(int face, int x, int y);
	glm::vec3 inline getVertex(int x, int y, int face);
	glm::vec3 inline getVertex(int x, int y, int face, float height);
	void addTriangle(int l, int f, int (&xs)[3], int (&ys)[3]);
	std::vector<std::vector<std::vector<float>>> heightmap;
	glm::mat4 faceTrans[6];
	//Vertices for different biomes
	std::vector<glm::vec3> vert_sea;
	std::vector<glm::vec3> vert_land;
	std::vector<glm::vec3> vert_rock;
	//Companion information for vertices
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> norm;
	std::vector<glm::vec3> tan;
	std::vector<glm::vec3> bitan;

	//Store the meshes at different Level of Detail
	//LOD       Face        GridX       GridY       Meshes
	std::vector<std::vector<std::vector<std::vector<PlanetMeshes>>>> LODS;
};

