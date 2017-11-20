#pragma once
#include "Renderable.h"
class UnindexedMesh :
	public Renderable {
public:
	UnindexedMesh();
	~UnindexedMesh();
	// Sets the mesh
	void setMesh(vector<glm::vec3> vertices, vector<glm::vec2> uvs, vector<glm::vec3> normals, vector<glm::vec3> tangents, vector<glm::vec3> bitangents);
	// Draws the mesh
	void render(Camera* cam, GLuint depthMap, glm::mat4 LSM);
	// Draws the mesh's shadow
	void renderShadow(GLuint p);
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
	bool useNormalTexture;
private:
	vector<glm::vec3> vertices;
	vector<glm::vec2> uvs;
	vector<glm::vec3> normals;
	vector<glm::vec3> tangents;
	vector<glm::vec3> bitangents;
	GLuint vertexArray;
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
};
