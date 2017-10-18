#include "Model.h"
#include "Mesh.h"
#include <tiny_obj_loader.h>
#include <stb_image.h>
#include <iostream>


Model::Model() {
}


Model::~Model() {
}


bool Model::loadModel(const char* path) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	//Stupid model loader doesn't seem to understand subfolders
	std::string baseDir = "";
	if (std::string(path).find_last_of("/\\") != std::string::npos) {
		baseDir = std::string(path).substr(0, std::string(path).find_last_of("/\\")) + "/";
	}
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path, baseDir.c_str());
	if (!err.empty()) {
		std::cerr << err << std::endl;
		return false;
	}

	if (!ret) {
		std::cerr << "Failed to load " << path << std::endl;
		return false;
	}
	for (tinyobj::shape_t s : shapes) {
		Mesh* m = new Mesh();
		m->setName(s.name);
		//Construct list for each mesh
		std::vector<glm::vec3> verts;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> norms;
		std::vector<glm::vec3> tans;
		std::vector<glm::vec3> bitans;
		for (unsigned int i = 0; i < s.mesh.indices.size(); i++) {
			int p = s.mesh.indices[i].vertex_index;
			glm::vec3 v = glm::vec3(attrib.vertices[3 * p + 0],
				attrib.vertices[3 * p + 1],
				attrib.vertices[3 * p + 2]);
			verts.push_back(v);
			p = s.mesh.indices[i].normal_index;
			v = glm::vec3(attrib.normals[3 * p + 0],
				attrib.normals[3 * p + 1],
				attrib.normals[3 * p + 2]);
			norms.push_back(v);
			p = s.mesh.indices[i].texcoord_index;
			glm::vec2 u = glm::vec2(attrib.texcoords[2 * p + 0],
				attrib.texcoords[2 * p + 1]);
			uvs.push_back(u);
		}
		//Compute tangents and bitangents
		computeTangentBasis(verts, uvs, norms, tans, bitans);
		std::vector<unsigned short> ind;
		std::vector<glm::vec3> v_out;
		std::vector<glm::vec2> u_out;
		std::vector<glm::vec3> n_out;
		std::vector<glm::vec3> t_out;
		std::vector<glm::vec3> b_out;
		indexVBO(verts, uvs, norms, tans, bitans, ind, v_out, u_out, n_out, t_out, b_out);
		m->setMesh(ind, v_out, u_out, n_out, t_out, b_out);
		tinyobj::material_t mat = materials[s.mesh.material_ids[0]];
		m->setDiffuse(OpenGLSetup::loadImage(baseDir + mat.diffuse_texname));
		m->setNormal(OpenGLSetup::loadImage(baseDir + mat.normal_texname));
		m->setSpecular(OpenGLSetup::loadImage(baseDir + mat.specular_texname));
		m->setEmission(OpenGLSetup::loadImage(baseDir + mat.emissive_texname));
		m->setShininess(mat.shininess);
		m->setParent(this);
	}
	return true;
}

void Model::indexVBO(
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
) {
	std::map<PackedVertex, unsigned short> VertexToOutIndex;

	// For each input vertex
	for (unsigned int i = 0; i<in_vertices.size(); i++) {

		PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };


		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex(packed, VertexToOutIndex, index);

		if (found) { // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
			out_tangents[index] += in_tangents[i];
			out_bitangents[index] += in_bitangents[i];
		} else { // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			out_tangents.push_back(in_tangents[i]);
			out_bitangents.push_back(in_bitangents[i]);
			unsigned short newindex = (unsigned short)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}

void Model::computeTangentBasis(
	// inputs
	std::vector<glm::vec3> & vertices,
	std::vector<glm::vec2> & uvs,
	std::vector<glm::vec3> & normals,
	// outputs
	std::vector<glm::vec3> & tangents,
	std::vector<glm::vec3> & bitangents
) {
	for (unsigned int i = 0; i<vertices.size(); i += 3) {
		// Shortcuts for vertices
		glm::vec3 & v0 = vertices[i + 0];
		glm::vec3 & v1 = vertices[i + 1];
		glm::vec3 & v2 = vertices[i + 2];
		// Shortcuts for UVs
		glm::vec2 & uv0 = uvs[i + 0];
		glm::vec2 & uv1 = uvs[i + 1];
		glm::vec2 & uv2 = uvs[i + 2];
		// Edges of the triangle : postion delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;
		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;
		// Set the same tangent for all three vertices of the triangle.
		// They will be merged later, in vboindexer.cpp
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		// Same thing for binormals
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}
	for (unsigned int i = 0; i<vertices.size(); i += 1) {
		glm::vec3 & n = normals[i];
		glm::vec3 & t = tangents[i];
		glm::vec3 & b = bitangents[i];
		// Gram-Schmidt orthogonalize
		t = glm::normalize(t - n * glm::dot(n, t));
		// Calculate handedness
		if (glm::dot(glm::cross(n, t), b) < 0.0f) {
			t = t * -1.0f;
		}
	}
}
