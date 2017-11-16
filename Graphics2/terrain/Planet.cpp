#include "Planet.h"
#include <random>
#include "../renderer/glm/gtc/matrix_transform.hpp"
//Generator settings
//For testing use low LOD
#define NODES_EXP 5
#define NUM_NODES ((1 << NODES_EXP) + 1)
#define MIN_Y (-0.15f)
#define MAX_Y 0.15f
#define ROUGHNESS 1.0f

//Biome settings
#define HEIGHT_SEA 0.05f
#define HEIGHT_ROCK 0.10f

//Enums for the face of the planet
#define FACE_POS_X 0
#define FACE_NEG_X 1
#define FACE_POS_Y 2
#define FACE_NEG_Y 3
#define FACE_POS_Z 4
#define FACE_NEG_Z 5


Planet::Planet() {
}


Planet::~Planet() {
}

/*
Generate a planet by creating 6 height maps that share edges.
Put these height maps in a cube and distort the cube into a sphere.

Height maps are generated using Diamond-Square Algorithm

x=0,y=0 is bottom left

*/

void Planet::generateTerrain() {
	//Allocate memory for heightmap
	for (unsigned int f = 0; f < 6; f++) {
		//For each face
		std::vector<std::vector<float>> face;
		for (unsigned int x = 0; x < NUM_NODES; x++) {
			//For each x value
			std::vector<float> xVec;
			for (unsigned int y = 0; y < NUM_NODES; y++) {
				xVec.push_back(0.0f);
			}
			face.push_back(xVec);
		}
		heightmap.push_back(face);
	}
	//Generate the heightmap
	diamondSquare();
	//Set up transformations for nodes
	createTransformations();

	/*
	For each level of detail calculate necessary size of grid (per face)
	Create mesh for each grid on each face at each LOD
	LOD0 = Every vertex (lots of grids needed to keep under 65k indices)
	LOD1 = Every other vertex
	LOD2 = Every 4th vertex
	Etc

	Maximum verts per grid = 65k indices = max of 4 indices per vertex
	Max verts per grid = 16k to be safe
	Define grid size to be constant (MAX_VERTS by MAX_VERTS)
	*/
	numGrids = static_cast<int>(ceil(static_cast<float>(NUM_NODES) / MAX_VERTS));
	for (int l = 0; l < NUM_LOD; l++) {
		//For each face
		for (int face = 0; face < 6; face++) {
			//For each grid cell
			for (int gridX = 0; gridX < numGrids; gridX++) {
				for (int gridY = 0; gridY < numGrids; gridY++) {
					//Calculate bounds of the grid
					int minX = MAX_VERTS * gridX;
					int minY = MAX_VERTS * gridY;
					int maxX = gridX == numGrids - 1 ? NUM_NODES : minX + MAX_VERTS;
					int maxY = gridY == numGrids - 1 ? NUM_NODES : minY + MAX_VERTS;
					generateGrid(l, face, minX, minY, maxX, maxY);
					makeMeshes(l, face, gridX, gridY);
				}
			}
		}
	}
}

//Optimise by tracking current + last face, only update visible faces?

//TODO Handle transformed planets

void Planet::updateVisible(SceneObject* s, glm::vec3 pos) {
	//Go over each grid and update lod
	for (int face = 0; face < 6; face++) {
		for (int gridX = 0; gridX < numGrids; gridX++) {
			int cX = gridX * MAX_VERTS + MAX_VERTS / 2;
			if (gridX == numGrids - 1) {
				cX = gridX * MAX_VERTS + (NUM_NODES - gridX * MAX_VERTS) / 2;
			}
			for (int gridY = 0; gridY < numGrids; gridY++) {
				int cY = gridY * MAX_VERTS + MAX_VERTS / 2;
				if (gridY == numGrids - 1) {
					cY = gridY * MAX_VERTS + (NUM_NODES - gridY * MAX_VERTS) / 2;
				}
				glm::vec3 off = pos - getVertex(cX, cY, face) * planetScale;
				float distSqr = glm::dot(off, off);
				int lod = NUM_LOD - 1;
				while (LOD_Distances[lod] > distSqr && lod>0) { lod--; }
				if (lod != lastLOD[face][gridX][gridY] || s != lastParent) {
					//Hide old LOD
					changeParent(LODS[lastLOD[face][gridX][gridY]][face][gridX][gridY], NULL);
					//Set new LOD visible
					changeParent(LODS[lod][face][gridX][gridY], s);
					//Update lastLOD
					lastLOD[face][gridX][gridY] = lod;
					lastParent = s;
				}
			}
		}
	}
}

void Planet::setLODS(float lods[NUM_LOD]) {
	//Copy values (square for cheaper distance checks)
	//No error checks, because its your own damn fault if it breaks
	for (int i = 0; i < NUM_LOD; i++) {
		LOD_Distances[i] = lods[i] * lods[i];
	}

}

void Planet::diamondSquare() {
	std::uniform_real_distribution<float> uni(MIN_Y, MAX_Y);
	std::default_random_engine rng(seed);
	//Create corners
	setNode(static_cast<float>(uni(rng)), FACE_NEG_Z, 0, 0);
	setNode(static_cast<float>(uni(rng)), FACE_NEG_Z, 0, NUM_NODES - 1);
	setNode(static_cast<float>(uni(rng)), FACE_NEG_Z, NUM_NODES - 1, 0);
	setNode(static_cast<float>(uni(rng)), FACE_NEG_Z, NUM_NODES - 1, NUM_NODES - 1);
	setNode(static_cast<float>(uni(rng)), FACE_POS_Z, 0, 0);
	setNode(static_cast<float>(uni(rng)), FACE_POS_Z, 0, NUM_NODES - 1);
	setNode(static_cast<float>(uni(rng)), FACE_POS_Z, NUM_NODES - 1, 0);
	setNode(static_cast<float>(uni(rng)), FACE_POS_Z, NUM_NODES - 1, NUM_NODES - 1);

	//Iteratively apply DSA
	int size = NUM_NODES - 1;
	float rand_var = (MAX_Y - MIN_Y) / 2;
	while (size > 1) {
		std::uniform_real_distribution<float> u(-rand_var, rand_var);
		//Diamond stage
		for (int f = 0; f < 6; f++) {
			for (int x = 0; x < (NUM_NODES - 1); x += size) {
				for (int y = 0; y < (NUM_NODES - 1); y += size) {
					//Get height of surrounding nodes
					float p1, p2, p3, p4;
					p1 = getNode(f, x, y);
					p2 = getNode(f, x, y + size);
					p3 = getNode(f, x + size, y + size);
					p4 = getNode(f, x + size, y);
					setNode((p1 + p2 + p3 + p4) / 4.0f + u(rng), f, x + size / 2, y + size / 2);
				}
			}
		}
		//Square stages
		for (int f = 0; f < 6; f++) {
			//(x + y - 1) ÷ y
			int s = (NUM_NODES - 1 + size / 2) / (size / 2);
			for (int x = 0; x < s; x++) {
				for (int z = 0; z < s; z++) {
					if (x % 2 == z % 2) {
						continue;
					}
					int px = x * (size / 2);
					int pz = z * (size / 2);
					float h = 0;
					h += getNode(f, px - (size / 2), pz);
					h += getNode(f, px + (size / 2), pz);
					h += getNode(f, px, pz - (size / 2));
					h += getNode(f, px, pz + (size / 2));
					h /= 4;
					setNode(h + u(rng), f, px, pz);
				}
			}
		}
		//Decrease size
		rand_var *= static_cast<float>(pow(2, -ROUGHNESS));
		size = size / 2;
	}
}

inline void Planet::createTransformations() {
	//Transformations to apply to each face
	//Centre planet on 0,0,0 (model space)
	float halfNodes = static_cast<float>(NUM_NODES - 1) / 2.0f;
	glm::mat4 pos = glm::translate(glm::mat4(1), glm::vec3(-halfNodes, halfNodes, -halfNodes));
	glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(1.0f / (NUM_NODES - 1)));

	faceTrans[FACE_POS_X] = scale * glm::rotate(glm::mat4(1), -glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1), glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;
	faceTrans[FACE_NEG_X] = scale * glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * pos;
	faceTrans[FACE_POS_Y] = scale * glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;
	faceTrans[FACE_NEG_Y] = scale * glm::rotate(glm::mat4(1), glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;
	faceTrans[FACE_POS_Z] = scale * glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;
	faceTrans[FACE_NEG_Z] = scale * glm::rotate(glm::mat4(1), -glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1), -glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;
}

inline void Planet::generateGrid(int l, int f, int minX, int minY, int maxX, int maxY) {
	int step = 1 << l;
	int nodesX = (maxX - minX) / step;
	int nodesY = (maxY - minY) / step;
	for (int y = 0; y <= nodesY; y++) {
		for (int x = y % 2; x <= nodesX; x += 2) {
			//Transform (x,y) to correct LOD
			int lX = x * step + minX;
			int lY = y * step + minY;
			int xs[3];
			int ys[3];
			if (x > 0) {
				if (y > 0) {
					//Pos
					xs[0] = lX;
					ys[0] = lY;
					//Negative Y
					xs[1] = lX;
					ys[1] = lY - step;
					//Negative X
					xs[2] = lX - step;
					ys[2] = lY;
					addTriangle(l, f, xs, ys);
				}
				if (y < nodesY) {
					//Pos
					xs[0] = lX;
					ys[0] = lY;
					//Negative X
					xs[1] = lX - step;
					ys[1] = lY;
					//Positive Y
					xs[2] = lX;
					ys[2] = lY + step;
					addTriangle(l, f, xs, ys);
				}
			}
			if (x < nodesX) {
				//Neighbours to the -y
				if (y > 0) {
					//Pos
					xs[0] = lX;
					ys[0] = lY;
					//Positive X
					xs[1] = lX + step;
					ys[1] = lY;
					//Negative Y
					xs[2] = lX;
					ys[2] = lY - step;
					addTriangle(l, f, xs, ys);
				}
				//Neighbours to the +y
				if (y < nodesY) {
					//Pos
					xs[0] = lX;
					ys[0] = lY;
					//Positive Y
					xs[1] = lX;
					ys[1] = lY + step;
					//Positive X
					xs[2] = lX + step;
					ys[2] = lY;
					addTriangle(l, f, xs, ys);
				}
			}
		}
	}
}

inline void Planet::makeMeshes(int l, int face, int gridX, int gridY) {
	PlanetMeshes meshes;
	if (LODS.size() <= l) {
		std::vector<std::vector<std::vector<PlanetMeshes>>> n;
		LODS.push_back(n);
	}
	if (LODS[l].size() <= face) {
		std::vector<std::vector<PlanetMeshes>> n;
		LODS[l].push_back(n);
	}
	if (LODS[l][face].size() <= gridX) {
		std::vector<PlanetMeshes> n;
		LODS[l][face].push_back(n);
	}
	if (LODS[l][face][gridX].size() <= gridY) {
		LODS[l][face][gridX].push_back(meshes);
	}
	if (lastLOD.size() <= face) {
		std::vector<std::vector<int>> last;
		lastLOD.push_back(last);
	}
	if (lastLOD[face].size() <= gridX) {
		std::vector<int> last;
		lastLOD[face].push_back(last);
	}
	if (lastLOD[face][gridX].size() <= gridY) {
		lastLOD[face][gridX].push_back(0);
	}
	std::vector<unsigned short> ind;
	std::vector<glm::vec3> o_vert;
	std::vector<glm::vec2> o_uv;
	std::vector<glm::vec3> o_norm;
	std::vector<glm::vec3> o_tan;
	std::vector<glm::vec3> o_bitan;
	//Calculate tangent + bitangent
	Model::computeTangentBasis(vert_sea, uv_sea, norm_sea, tan, bitan);
	//Index vertices
	Model::indexVBO(vert_sea, uv_sea, norm_sea, tan, bitan, ind, o_vert, o_uv, o_norm, o_tan, o_bitan);
	//Set mesh
	if (ind.size() > 0) {
		Mesh* m = new Mesh();
		m->setMesh(ind, o_vert, o_uv, o_norm, o_tan, o_bitan);
		m->useNormalTexture = false;
		meshes.sea = m;
		meshes.sea->setDiffuse(OpenGLSetup::loadImage("assets/testing/water.png"));
	} else {
		meshes.sea = NULL;
	}
	ind.clear();
	vert_sea.clear();
	uv_sea.clear();
	norm_sea.clear();
	tan.clear();
	bitan.clear();
	o_vert.clear();
	o_uv.clear();
	o_norm.clear();
	o_tan.clear();
	o_bitan.clear();
	//Calculate tangent + bitangent
	Model::computeTangentBasis(vert_land, uv_land, norm_land, tan, bitan);
	//Index vertices
	Model::indexVBO(vert_land, uv_land, norm_land, tan, bitan, ind, o_vert, o_uv, o_norm, o_tan, o_bitan);
	//Set mesh
	if (ind.size() > 0) {
		Mesh* m = new Mesh();
		m->setMesh(ind, o_vert, o_uv, o_norm, o_tan, o_bitan);
		m->useNormalTexture = false;
		meshes.grass = m;
		meshes.grass->setDiffuse(OpenGLSetup::loadImage("assets/testing/grass.png"));
	} else {
		meshes.grass = NULL;
	}
	ind.clear();
	vert_land.clear();
	uv_land.clear();
	norm_land.clear();
	tan.clear();
	bitan.clear();
	o_vert.clear();
	o_uv.clear();
	o_norm.clear();
	o_tan.clear();
	o_bitan.clear();
	//Calculate tangent + bitangent
	Model::computeTangentBasis(vert_rock, uv_rock, norm_rock, tan, bitan);
	//Index vertices
	Model::indexVBO(vert_rock, uv_rock, norm_rock, tan, bitan, ind, o_vert, o_uv, o_norm, o_tan, o_bitan);
	//Set mesh
	if (ind.size() > 0) {
		Mesh* m = new Mesh();
		m->setMesh(ind, o_vert, o_uv, o_norm, o_tan, o_bitan);
		m->useNormalTexture = false;
		meshes.rock = m;
		meshes.rock->setDiffuse(OpenGLSetup::loadImage("assets/testing/rock.png"));
	} else {
		meshes.rock = NULL;
	}
	ind.clear();
	vert_rock.clear();
	uv_rock.clear();
	norm_rock.clear();
	tan.clear();
	bitan.clear();
	o_vert.clear();
	o_uv.clear();
	o_norm.clear();
	o_tan.clear();
	o_bitan.clear();
	LODS[l][face][gridX][gridY] = meshes;
}

void Planet::setNode(float value, unsigned int face, unsigned int x, unsigned int y) {
	//I think opengl's backwards z axis messed something up and this sort of fixes it
	heightmap[face][y][x] = value;
	if (face == FACE_POS_X) {
		//Left edge
		if (x == 0) {
			heightmap[FACE_POS_Z][y][NUM_NODES - 1] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			heightmap[FACE_NEG_Z][y][0] = value;
		}
		//Bottom edge
		if (y == 0) {
			heightmap[FACE_NEG_Y][NUM_NODES - 1 - x][NUM_NODES - 1] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			heightmap[FACE_POS_Y][x][NUM_NODES - 1] = value;
		}
	}
	if (face == FACE_NEG_X) {
		//Left edge
		if (x == 0) {
			heightmap[FACE_NEG_Z][y][NUM_NODES - 1] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			heightmap[FACE_POS_Z][y][0] = value;
		}
		//Bottom edge
		if (y == 0) {
			heightmap[FACE_NEG_Y][x][0] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			heightmap[FACE_POS_Y][NUM_NODES - 1 - x][0] = value;
		}
	}
	if (face == FACE_POS_Y) {
		//Left edge
		if (x == 0) {
			heightmap[FACE_NEG_X][NUM_NODES - 1][NUM_NODES - 1 - y] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			heightmap[FACE_POS_X][NUM_NODES - 1][y] = value;
		}
		//Bottom edge
		if (y == 0) {
			heightmap[FACE_NEG_Z][NUM_NODES - 1][x] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			heightmap[FACE_POS_Z][NUM_NODES - 1][NUM_NODES - 1 - x] = value;
		}
	}
	if (face == FACE_NEG_Y) {
		//Left edge
		if (x == 0) {
			heightmap[FACE_NEG_X][0][y] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			heightmap[FACE_POS_X][0][NUM_NODES - 1 - y] = value;
		}
		//Bottom edge
		if (y == 0) {
			heightmap[FACE_POS_Z][0][x] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			heightmap[FACE_NEG_Z][0][x] = value;
		}
	}
	if (face == FACE_POS_Z) {
		//Left edge
		if (x == 0) {
			heightmap[FACE_NEG_X][y][NUM_NODES - 1] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			heightmap[FACE_POS_X][y][0] = value;
		}
		//Bottom edge
		if (y == 0) {
			heightmap[FACE_NEG_Y][NUM_NODES - 1][x] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			heightmap[FACE_POS_Y][0][x] = value;
		}
	}
	if (face == FACE_NEG_Z) {
		//Left edge
		if (x == 0) {
			heightmap[FACE_POS_X][y][NUM_NODES - 1] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			heightmap[FACE_NEG_X][y][0] = value;
		}
		//Bottom edge
		if (y == 0) {
			heightmap[FACE_NEG_Y][0][NUM_NODES - 1 - x] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			heightmap[FACE_POS_Y][NUM_NODES - 1][NUM_NODES - 1 - x] = value;
		}
	}
}

float Planet::getNode(int face, int x, int y) {
	//Handle wrapping (mostly, don't expect large numbers to work)
	if (face == FACE_POS_X) {
		if (x < 0) {
			face = FACE_POS_Z;
			x += NUM_NODES - 1;
		} else if (x > NUM_NODES - 1) {
			face = FACE_NEG_Z;
			x -= NUM_NODES - 1;
		} else if (y < 0) {
			face = FACE_NEG_Y;
			int tmp = NUM_NODES - 1 + y;
			y = NUM_NODES - 1 - x;
			x = tmp;
		} else if (y > NUM_NODES - 1) {
			face = FACE_POS_Y;
			int tmp = x;
			x = NUM_NODES - 1 - (y - (NUM_NODES - 1));
			y = tmp;
		}
	} else if (face == FACE_NEG_X) {
		if (x < 0) {
			face = FACE_NEG_Z;
			x += NUM_NODES - 1;
		} else if (x > NUM_NODES - 1) {
			face = FACE_POS_Z;
			x -= NUM_NODES - 1;
		} else if (y < 0) {
			face = FACE_NEG_Y;
			int tmp = -y;
			y = x;
			x = tmp;
		} else if (y > NUM_NODES - 1) {
			face = FACE_POS_Y;
			int tmp = (NUM_NODES - 1) - x;
			x = y - (NUM_NODES - 1);
			y = tmp;
		}
	} else if (face == FACE_POS_Y) {
		if (x < 0) {
			face = FACE_NEG_X;
			int tmp = (NUM_NODES - 1) + x;
			x = (NUM_NODES - 1) - y;
			y = tmp;
		} else if (x > NUM_NODES - 1) {
			face = FACE_POS_X;
			int tmp = NUM_NODES - 1 - (x - (NUM_NODES - 1));
			x = y;
			y = tmp;
		} else if (y < 0) {
			face = FACE_POS_Z;
			y += NUM_NODES - 1;
		} else if (y > NUM_NODES - 1) {
			face = FACE_NEG_Z;
			x = NUM_NODES - 1 - x;
			y = NUM_NODES - 1 - (y - (NUM_NODES - 1));
		}
	} else if (face == FACE_NEG_Y) {
		if (x < 0) {
			face = FACE_NEG_X;
			int tmp = -x;
			x = y;
			y = tmp;
		} else if (x > NUM_NODES - 1) {
			face = FACE_POS_X;
			int tmp = x - (NUM_NODES - 1);
			x = NUM_NODES - 1 - y;
			y = tmp;
		} else if (y < 0) {
			face = FACE_NEG_Z;
			x = NUM_NODES - 1 - x;
			y = -y;
		} else if (y > NUM_NODES - 1) {
			face = FACE_NEG_Z;
			y -= NUM_NODES - 1;
		}
	} else if (face == FACE_POS_Z) {
		if (x < 0) {
			face = FACE_NEG_X;
			x += NUM_NODES - 1;
		} else if (x > NUM_NODES - 1) {
			face = FACE_POS_X;
			x -= NUM_NODES - 1;
		} else if (y < 0) {
			face = FACE_NEG_Y;
			y = NUM_NODES - 1 + y;
		} else if (y > NUM_NODES - 1) {
			face = FACE_POS_Y;
			y -= NUM_NODES - 1;
		}
	} else if (face == FACE_NEG_Z) {
		if (x < 0) {
			face = FACE_POS_X;
			x += NUM_NODES - 1;
		} else if (x > NUM_NODES - 1) {
			face = FACE_NEG_X;
			x -= NUM_NODES - 1;
		} else if (y < 0) {
			face = FACE_NEG_Y;
			x = NUM_NODES - 1 - x;
			y = -y;
		} else if (y > NUM_NODES - 1) {
			face = FACE_POS_Y;
			x = NUM_NODES - 1 - x;
			y = NUM_NODES - 1 - (y - (NUM_NODES - 1));
		}
	}
	if (x >= heightmap[face].size() || x < 0 || y >= heightmap[face][x].size() || y < 0) {
		//Damn it go for another pass
		return getNode(face, x, y);
	}
	return heightmap[face][x][y];
}

glm::vec3 Planet::getVertex(int x, int y, int face) {
	//Get position on sphere
	glm::vec3 p = glm::vec3(faceTrans[face] * glm::vec4(x, 0.0f, y, 1.0f));
	p = glm::normalize(p);
	//Extrude by heightmap
	float height = 1.0f + getNode(face, x, y);
	p = p * height;
	return p;
}

glm::vec3 Planet::getVertex(int x, int y, int face, float height) {
	//Get position on sphere
	glm::vec3 p = glm::vec3(faceTrans[face] * glm::vec4(x, 0.0f, y, 1.0f));
	p = glm::normalize(p);
	//Extrude by heightmap
	p = p * (height + 1.0f);
	return p;
}

void Planet::addTriangle(int l, int f, int (&xs)[3], int (&ys)[3]) {
	bool addSea = false;
	bool addLand = false;
	bool addRock = false;
	//Count how many vertices are land height(-) and how many are rock height(+)
	int landVsRock = 0;
	for (int i = 0; i < 3; i++) {
		float h = getNode(f, xs[i], ys[i]);
		if (h < HEIGHT_SEA) {
			addSea = true;
		} else if(h > HEIGHT_ROCK) {
			landVsRock++;
		} else {
			landVsRock--;
		}
	}
	addLand = landVsRock <= 0;
	addRock = landVsRock > 0;
	//If any point is below sea level add all to sea (setting height to sea level)
	if (addSea) {
		for (int i = 0; i < 3; i++) {
			glm::vec3 v = getVertex(xs[i], ys[i], f, HEIGHT_SEA);
			vert_sea.push_back(v);
			uv_sea.push_back(glm::vec2(static_cast<float>(xs[i]) / (NUM_NODES), static_cast<float>(ys[i]) / (NUM_NODES)));
			norm_sea.push_back(glm::normalize(v));
		}
	}
	//If any point is above sea level add to land
	if (addLand) {
		for (int i = 0; i < 3; i++) {
			glm::vec3 v = getVertex(xs[i], ys[i], f);
			vert_land.push_back(v);
			uv_land.push_back(glm::vec2(static_cast<float>(xs[i]) / (NUM_NODES), static_cast<float>(ys[i]) / (NUM_NODES)));
			norm_land.push_back(glm::normalize(v));
		}
	}
	//If any point is a mountain add to rock
	if (addRock) {
		for (int i = 0; i < 3; i++) {
			glm::vec3 v = getVertex(xs[i], ys[i], f);
			vert_rock.push_back(v);
			uv_rock.push_back(glm::vec2(static_cast<float>(xs[i]) / (NUM_NODES), static_cast<float>(ys[i]) / (NUM_NODES)));
			norm_rock.push_back(glm::normalize(v));
		}
	}
}

inline void Planet::changeParent(PlanetMeshes& meshes, SceneObject* parent) {
	if (meshes.sea) {
		meshes.sea->setParent(parent);
	}
	if (meshes.grass) {
		meshes.grass->setParent(parent);
	}
	if (meshes.rock) {
		meshes.rock->setParent(parent);
	}
}
