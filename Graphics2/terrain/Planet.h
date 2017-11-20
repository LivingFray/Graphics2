#pragma once
#include <vector>
#include "..\renderer\Mesh.h"
#include "..\renderer\Scene.h"

//Graphical settings (LOD)
#define NUM_LOD 4
#define MAX_VERTS (1 << 4)

class Planet {
public:
	Planet();
	~Planet();
	void generateTerrain();
	//Updates the list of meshes that can be seen
	void updateVisible(SceneObject* highLod, SceneObject* lowLod, glm::vec3 pos);

	void setLODS(float lods[NUM_LOD]);

	float planetScale = 1.0f;
	float lowLodScale = 1.0f;
	unsigned int seed;
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
	float getNode(int face, int x, int y);
	glm::vec3 inline getVertex(int x, int y, int face);
	glm::vec3 inline getVertex(int x, int y, int face, float height);
	void inline addTriangle(int l, int f, int (&xs)[3], int (&ys)[3]);


	//LOD helper function
	void inline changeParent(PlanetMeshes &m, SceneObject* parent);

	std::vector<std::vector<std::vector<float>>> heightmap;
	glm::mat4 faceTrans[6];
	//Vertices for different biomes
	std::vector<glm::vec3> vert_sea;
	std::vector<glm::vec3> vert_land;
	std::vector<glm::vec3> vert_rock;
	std::vector<glm::vec2> uv_sea;
	std::vector<glm::vec2> uv_land;
	std::vector<glm::vec2> uv_rock;
	std::vector<glm::vec3> norm_sea;
	std::vector<glm::vec3> norm_land;
	std::vector<glm::vec3> norm_rock;
	//Companion information for vertices
	std::vector<glm::vec3> tan;
	std::vector<glm::vec3> bitan;

	//Store the meshes at different Level of Detail
	//LOD       Face        GridX       GridY       Meshes
	std::vector<std::vector<std::vector<std::vector<PlanetMeshes>>>> LODS;
	//Stores the last LOD a grid was rendered at
	//Face     GridX       GridY
	std::vector<std::vector<std::vector<int>>> lastLOD;
	SceneObject* lastParent = NULL;
};

