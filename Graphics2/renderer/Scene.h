#pragma once
/*
A scene is everything that is rendered, multiple scenes can be rendered at once
by combining multiple cameras (example use for this is drawing planets with one
scene and local objects like spaceships with another)
*/
#include <set>
#include <string>
#include "SceneObject.h"
#include "Renderable.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

using std::string;
using std::set;

class Scene: public SceneObject{
public:
	Scene();
	~Scene();
	const set<Renderable*>& getRenderables() { return renderables; };
	void loadSkybox(string posX, string negX, string posY, string negY, string posZ, string negZ);
	void renderSkybox(Camera* c);
	void updateLights();
	DirectionalLight* getDirectionalLight();
	glm::vec3 ambientLight;
	glm::vec3 skyColour;
	float skyAmount;
private:
	//Special rendering list
	set<Renderable*> renderables;
	DirectionalLight* dirLight;
	set<PointLight*> pointLights;
	set<SpotLight*> spotLights;
	friend Renderable;
	friend DirectionalLight;
	friend PointLight;
	friend SpotLight;
	GLuint skybox;
	Shader skyboxShader;
	Shader meshShader;
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint viewUniform;
	GLuint projUniform;
	GLuint cubeSampler;
	GLuint skyColourUniform;
	GLuint skyAmountUniform;
	static constexpr GLfloat vertexData[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
};
