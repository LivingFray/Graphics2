#include "Planet.h"
#include <random>
#include "../renderer/glm/gtc/matrix_transform.hpp"
//For testing use low LOD
#define NODES_EXP 4
#define NUM_NODES (1 << NODES_EXP) + 1
#define MIN_Y (-0.15)
#define MAX_Y 0.15
#define ROUGHNESS 1.0

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
	std::uniform_real_distribution<float> uni(MIN_Y, MAX_Y);
	std::default_random_engine rng;
	//Generate Vectors to hold data
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
		faces.push_back(face);
	}
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
	//Nodes on the edge get recalculated a few times, but I really don't want to fix that
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
			//(x + y - 1) � y
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
	//Convert to mesh (temp?)
	std::vector<glm::vec3> vert;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> norm;
	std::vector<glm::vec3> tan;
	std::vector<glm::vec3> bitan;

	//Transformations to apply to each face
	//Centre planet on 0,0,0 (model space)
	float halfNodes = static_cast<float>(NUM_NODES - 1) / 2.0f;
	glm::mat4 pos = glm::translate(glm::mat4(1), glm::vec3(-halfNodes, halfNodes, -halfNodes));
	glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(1.0f / (NUM_NODES - 1)));
	//These transformations got somewhat out of hand...
	glm::mat4 faceTrans[6];

	faceTrans[FACE_POS_X] = glm::rotate(glm::mat4(1), -glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1), glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;
	faceTrans[FACE_NEG_X] = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * pos;
	faceTrans[FACE_POS_Y] = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;
	faceTrans[FACE_NEG_Y] = glm::rotate(glm::mat4(1), glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;
	faceTrans[FACE_POS_Z] = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;
	faceTrans[FACE_NEG_Z] = glm::rotate(glm::mat4(1), -glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1), -glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;

	/*
	For each face generate face
	Each node has 4 associated faces
	For even z: start at beginning, for odd start 1 in
	Add "diamond" centred on every other position
	*/
	for (int f = 0; f < 6; f++) {
		for (unsigned int y = 0; y < NUM_NODES; y++) {
			for (unsigned int x = y % 2; x < NUM_NODES; x += 2) {
				if (x > 0) {
					if (y > 0) {
						//Pos
						vert.push_back(getVertex(scale * faceTrans[f], x, y, f));
						//Negative Y
						vert.push_back(getVertex(scale * faceTrans[f], x, y - 1, f));
						//Negative X
						vert.push_back(getVertex(scale * faceTrans[f], x - 1, y, f));
						uv.push_back(glm::vec2(static_cast<float>(x) / (NUM_NODES), static_cast<float>(y) / (NUM_NODES)));
						uv.push_back(glm::vec2(static_cast<float>(x) / (NUM_NODES), static_cast<float>(y - 1) / (NUM_NODES)));
						uv.push_back(glm::vec2(static_cast<float>(x - 1) / (NUM_NODES), static_cast<float>(y) / (NUM_NODES)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
					}
					if (y < NUM_NODES - 1) {
						//Pos
						vert.push_back(getVertex(scale * faceTrans[f], x, y, f));
						//Negative X
						vert.push_back(getVertex(scale * faceTrans[f], x - 1, y, f));
						//Positive Y
						vert.push_back(getVertex(scale * faceTrans[f], x, y + 1, f));
						uv.push_back(glm::vec2(static_cast<float>(x) / (NUM_NODES), static_cast<float>(y) / (NUM_NODES)));
						uv.push_back(glm::vec2(static_cast<float>(x - 1) / (NUM_NODES), static_cast<float>(y) / (NUM_NODES)));
						uv.push_back(glm::vec2(static_cast<float>(x) / (NUM_NODES), static_cast<float>(y + 1) / (NUM_NODES)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));

					}
				}
				if (x < NUM_NODES - 1) {
					//Neighbours to the -y
					if (y > 0) {
						//Pos
						vert.push_back(getVertex(scale * faceTrans[f], x, y, f));
						//Positive X
						vert.push_back(getVertex(scale * faceTrans[f], x + 1, y, f));
						//Negative Y
						vert.push_back(getVertex(scale * faceTrans[f], x, y - 1, f));
						uv.push_back(glm::vec2(static_cast<float>(x) / (NUM_NODES), static_cast<float>(y) / (NUM_NODES)));
						uv.push_back(glm::vec2(static_cast<float>(x + 1) / (NUM_NODES), static_cast<float>(y) / (NUM_NODES)));
						uv.push_back(glm::vec2(static_cast<float>(x) / (NUM_NODES), static_cast<float>(y - 1) / (NUM_NODES)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
					}
					//Neighbours to the +y
					if (y < NUM_NODES - 1) {
						//Pos
						vert.push_back(getVertex(scale * faceTrans[f], x, y, f));
						//Positive Y
						vert.push_back(getVertex(scale * faceTrans[f], x, y + 1, f));
						//Positive X
						vert.push_back(getVertex(scale * faceTrans[f], x + 1, y, f));
						uv.push_back(glm::vec2(static_cast<float>(x) / (NUM_NODES), static_cast<float>(y) / (NUM_NODES)));
						uv.push_back(glm::vec2(static_cast<float>(x) / (NUM_NODES), static_cast<float>(y + 1) / (NUM_NODES)));
						uv.push_back(glm::vec2(static_cast<float>(x + 1) / (NUM_NODES), static_cast<float>(y) / (NUM_NODES)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
						norm.push_back(glm::vec3(faceTrans[f] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
					}
				}
				//TODO: Fix uv and normals
			}
		}
	}

	std::vector<unsigned short> ind;
	std::vector<glm::vec3> o_vert;
	std::vector<glm::vec2> o_uv;
	std::vector<glm::vec3> o_norm;
	std::vector<glm::vec3> o_tan;
	std::vector<glm::vec3> o_bitan;
	//Calculate tangent + bitangent
	Model::computeTangentBasis(vert, uv, norm, tan, bitan);
	//Index vertices
	Model::indexVBO(vert, uv, norm, tan, bitan, ind, o_vert, o_uv, o_norm, o_tan, o_bitan);
	//Set mesh
	planet.setMesh(ind, o_vert, o_uv, o_norm, o_tan, o_bitan);
}

void Planet::setNode(float value, unsigned int face, unsigned int x, unsigned int y) {
	//I think opengl's backwards z axis messed something up and this sort of fixes it
	faces[face][y][x] = value;
	if (face == FACE_POS_X) {
		//Left edge
		if (x == 0) {
			faces[FACE_POS_Z][y][NUM_NODES - 1] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			faces[FACE_NEG_Z][y][0] = value;
		}
		//Bottom edge
		if (y == 0) {
			faces[FACE_NEG_Y][NUM_NODES - 1 - x][NUM_NODES - 1] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			faces[FACE_POS_Y][x][NUM_NODES - 1] = value;
		}
	}
	if (face == FACE_NEG_X) {
		//Left edge
		if (x == 0) {
			faces[FACE_NEG_Z][y][NUM_NODES - 1] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			faces[FACE_POS_Z][y][0] = value;
		}
		//Bottom edge
		if (y == 0) {
			faces[FACE_NEG_Y][x][0] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			faces[FACE_POS_Y][NUM_NODES - 1 - x][0] = value;
		}
	}
	if (face == FACE_POS_Y) {
		//Left edge
		if (x == 0) {
			faces[FACE_NEG_X][NUM_NODES - 1][NUM_NODES - 1 - y] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			faces[FACE_POS_X][NUM_NODES - 1][y] = value;
		}
		//Bottom edge
		if (y == 0) {
			faces[FACE_NEG_Z][NUM_NODES - 1][x] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			faces[FACE_POS_Z][NUM_NODES - 1][NUM_NODES - 1 - x] = value;
		}
	}
	if (face == FACE_NEG_Y) {
		//Left edge
		if (x == 0) {
			faces[FACE_NEG_X][0][y] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			faces[FACE_POS_X][0][NUM_NODES - 1 - y] = value;
		}
		//Bottom edge
		if (y == 0) {
			faces[FACE_POS_Z][0][x] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			faces[FACE_NEG_Z][0][x] = value;
		}
	}
	if (face == FACE_POS_Z) {
		//Left edge
		if (x == 0) {
			faces[FACE_NEG_X][y][NUM_NODES - 1] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			faces[FACE_POS_X][y][0] = value;
		}
		//Bottom edge
		if (y == 0) {
			faces[FACE_NEG_Y][NUM_NODES - 1][x] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			faces[FACE_POS_Y][0][x] = value;
		}
	}
	if (face == FACE_NEG_Z) {
		//Left edge
		if (x == 0) {
			faces[FACE_POS_X][y][NUM_NODES - 1] = value;
		}
		//Right edge
		if (x == NUM_NODES - 1) {
			faces[FACE_NEG_X][y][0] = value;
		}
		//Bottom edge
		if (y == 0) {
			faces[FACE_NEG_Y][0][NUM_NODES - 1 - x] = value;
		}
		//Top edge
		if (y == NUM_NODES - 1) {
			faces[FACE_POS_Y][NUM_NODES - 1][NUM_NODES - 1 - x] = value;
		}
	}
}

float Planet::getNode(int face, int x, int y) {
	int sx = x;
	int sy = y;
	int sface = face;
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
	return faces[face][x][y];
}

glm::vec3 Planet::getVertex(glm::mat4 trans, int x, int y, int face) {
	//Get position on sphere
	glm::vec3 p = glm::vec3(trans * glm::vec4(x, 0.0f, y, 1.0f));
	p = glm::normalize(p);
	//Extrude by heightmap
	float height = 1.0f + faces[face][x][y];
	p = p * height;
	return p;
}