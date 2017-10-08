#pragma once
/*
A complex shape loaded from an obj file
*/
#include "Renderable.h"
#include "Model.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

class Mesh :
	public Renderable {
public:
	Mesh();
	virtual ~Mesh();
	// Sets the mesh
	void setMesh(vector<unsigned short> indices, vector<glm::vec3> vertices, vector<glm::vec2> uvs, vector<glm::vec3> normals, vector<glm::vec3> tangents, vector<glm::vec3> bitangents);
	// Draws the mesh
	void render(Camera* cam);
	// Sets the shininess of the mesh
	void setShininess(float shininess);
	// Sets the texture of the mesh
	void setDiffuse(GLuint diffuse);
	// Sets the specular map of the mesh
	void setSpecular(GLuint specular);
	// Sets the emission map of the mesh
	void setEmission(GLuint emission);
	// Sets the normal map of the mesh
	void setNormal(GLuint normal);
	// Sets the name of the mesh
	void setName(string name);
	// Gets the name of the mesh
	string getName() { return name; };
	// Sets the model the mesh belongs to
	void setModel(Model* m);
private:
	vector<unsigned short> indices;
	vector<glm::vec3> vertices;
	vector<glm::vec2> uvs;
	vector<glm::vec3> normals;
	vector<glm::vec3> tangents;
	vector<glm::vec3> bitangents;
	GLuint vertexArray;
	GLuint elementBuffer;
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalBuffer;
	GLuint tangentBuffer;
	GLuint bitangentBuffer;
	GLuint program;
	GLuint diffuse;
	GLuint specular;
	float shininess;
	GLuint emission;
	GLuint normal;
	string name;
	Model* model;
};

