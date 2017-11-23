#pragma once
/*
A model contains meshes that make up one model
*/
#include "OpenGLSetup.h"
#include <vector>
#include "SceneObject.h"
#include "Octree.h"
#include <tiny_obj_loader.h>


using std::vector;

class Mesh;

class Model :
	public SceneObject {
public:
	Model();
	~Model();
	// Loads the model from an obj file, returns true on success
	bool loadModel(const char* path);
	// Creates octrees for the children of the model
	void createOctrees(int maxDepth);
	// Checks if the octree collides with anything
	bool collides(Octree* other);
	//"Borrowed" from opengl-tutorials
	static void indexVBO(
		std::vector<glm::vec3> & in_vertices,
		std::vector<glm::vec2> & in_uvs,
		std::vector<glm::vec3> & in_normals,
		std::vector<glm::vec3> & in_tangents,
		std::vector<glm::vec3> & in_bitangents,
		std::vector<unsigned short> & out_indices,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals,
		std::vector<glm::vec3> & out_tangents,
		std::vector<glm::vec3> & out_bitangents
	);
	static void indexVBO(
		std::vector<glm::vec3> & in_vertices,
		std::vector<glm::vec2> & in_uvs,
		std::vector<glm::vec3> & in_normals,

		std::vector<unsigned short> & out_indices,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals
	);
	static void computeTangentBasis(
		// inputs
		std::vector<glm::vec3> & vertices,
		std::vector<glm::vec2> & uvs,
		std::vector<glm::vec3> & normals,
		// outputs
		std::vector<glm::vec3> & tangents,
		std::vector<glm::vec3> & bitangents
	);
private:
	struct PackedVertex {
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		bool operator<(const PackedVertex that) const {
			return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
		};
	};
	static bool getSimilarVertexIndex(
		PackedVertex & packed,
		std::map<PackedVertex, unsigned short> & VertexToOutIndex,
		unsigned short & result
	) {
		std::map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
		if (it == VertexToOutIndex.end()) {
			return false;
		} else {
			result = it->second;
			return true;
		}
	}
	std::vector<Mesh*> meshes;
};

