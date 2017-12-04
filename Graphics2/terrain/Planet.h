#pragma once
#include <vector>
#include "..\renderer\Mesh.h"
#include "..\renderer\Scene.h"
#include <unordered_set>
#include <thread>

//Graphical settings (LOD)
#define NUM_LOD 4
#define MAX_VERTS (1 << 4)

class Planet {
public:
	Planet();
	~Planet();
	void generateTerrain(int octDepth);
	//Updates the list of meshes that can be seen
	void updateVisible(SceneObject* highLod, SceneObject* lowLod, glm::vec3 pos, std::unordered_set<Mesh*> &highPoly);

	void setLODS(float lods[NUM_LOD]);

	float planetScale = 1.0f;
	float lowLodScale = 1.0f;
	unsigned int seed;

	void setNodeExp(int exp) { if (exp > 0) { nodesExp = exp; numNodes = (1 << exp) + 1; } }
	void setMinY(float y) { minHeight = y; }
	void setMaxY(float y) { maxHeight = y; }
	void setRoughness(float r) { roughness = r; }
	void setSeaHeight(float h) { heightSea = h; }
	void setRockHeight(float h) { heightRock = h; }
	void setSeaTexture(GLuint tex) { seaTex = tex; }
	void setLandTexture(GLuint tex) { landTex = tex; }
	void setRockTexture(GLuint tex) { rockTex = tex; }
	void setSeaSpecular(GLuint tex) { seaSpec = tex; }
	void setLandSpecular(GLuint tex) { landSpec = tex; }
	void setRockSpecular(GLuint tex) { rockSpec = tex; }

private:
	struct PlanetMeshes {
		Mesh* sea;
		Mesh* grass;
		Mesh* rock;
	};
	//Maximum distance at which that LOD is used
	float LOD_Distances[NUM_LOD] = { 0.1f, 0.5f, 1.0f, 1.5f };
	//The number of grids in each direction of each face
	int numGrids;

	//Terrain generation helper methods
	void inline diamondSquare();
	void inline createTransformations();
	void inline generateGrid(int l, int face, int minX, int minY, int maxX, int maxY);
	void inline makeMeshes(int l, int face, int gridX, int gridY);
	void inline setNode(float value, unsigned int face, unsigned int x, unsigned int y);
	void moveInBounds(int &face, int &x, int &y);
	void moveInBoundsGrid(int &face, int &x, int &y);
	float getNode(int face, int x, int y);
	glm::vec3 inline getVertex(int x, int y, int face);
	glm::vec3 inline getVertex(int x, int y, int face, float height);
	void inline addTriangle(int l, int f, int (&xs)[6], int (&ys)[6]);


	//LOD helper function
	void inline changeParent(PlanetMeshes &m, SceneObject* parent);

	std::vector<std::vector<std::vector<float>>> heightmap;
	glm::mat4 faceTrans[6];
	//Vertices for different biomes
	std::vector<unsigned short> ind_sea;
	std::vector<unsigned short> ind_land;
	std::vector<unsigned short> ind_rock;
	std::vector<glm::vec3> vert_sea;
	std::vector<glm::vec3> vert_land;
	std::vector<glm::vec3> vert_rock;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> norm;
	int nodesInGrid;

	//Store the meshes at different Level of Detail
	//LOD       Face        GridX       GridY       Meshes
	std::vector<std::vector<std::vector<std::vector<PlanetMeshes>>>> LODS;
	//Stores the last LOD a grid was rendered at
	//Face     GridX       GridY
	std::vector<std::vector<std::vector<int>>> lastLOD;

	//Last position scene was updated from
	glm::vec3 lastPos;
	//Threads used to speed up octree generation
	std::unordered_set<std::thread*> threads;

	//Generator settings
	int nodesExp = 7;
	int numNodes = (1 << 7) + 1;
	float minHeight = -0.15f;
	float maxHeight = 0.15f;
	float roughness = 1.0f;
	float heightSea = 0.05f;
	float heightRock = 0.1f;
	GLuint seaTex = 0;
	GLuint landTex = 0;
	GLuint rockTex = 0;
	GLuint seaSpec = 0;
	GLuint landSpec = 0;
	GLuint rockSpec = 0;
};

