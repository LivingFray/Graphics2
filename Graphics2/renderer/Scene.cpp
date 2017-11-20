#include "Scene.h"
#include <stb_image.h>


Scene::Scene() {
	ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);
	skyColour = glm::vec3(33.0f / 255.0f, 120.0f / 255.0f, 224.0f / 255.0f);
	skyAmount = 0.0f;
	//Skybox related things
	skybox = 0;
	skyboxShader = Shader("shaders/skybox.vert", "shaders/skybox.frag");
	meshShader = Shader("shaders/multiLight.vert", "shaders/multiLight.frag");
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), &vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	viewUniform = glGetUniformLocation(skyboxShader.getProgram(), "view");
	projUniform = glGetUniformLocation(skyboxShader.getProgram(), "projection");
	cubeSampler = glGetUniformLocation(skyboxShader.getProgram(), "skybox");
	skyColourUniform = glGetUniformLocation(skyboxShader.getProgram(), "skyColour");
	skyAmountUniform = glGetUniformLocation(skyboxShader.getProgram(), "skyAmount");
	glBindVertexArray(0);
}


Scene::~Scene() {
	if (skybox) {
		glDeleteTextures(1, &skybox);
	}
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
}

void Scene::loadSkybox(string posX, string negX, string posY, string negY, string posZ, string negZ) {
	if (skybox) {
		glDeleteTextures(1, &skybox);
		skybox = 0;
	}
	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1, &skybox);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	int width, height, channels;
	unsigned char* data = stbi_load(posX.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	data = stbi_load(negX.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	//stbi_set_flip_vertically_on_load(true);
	data = stbi_load(posY.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	data = stbi_load(negY.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	//stbi_set_flip_vertically_on_load(false);
	data = stbi_load(negZ.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	data = stbi_load(posZ.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	stbi_set_flip_vertically_on_load(true);
}

void Scene::renderSkybox(Camera* c) {
	if (skybox) {
		//Set shaders
		glUseProgram(skyboxShader.getProgram());
		glDepthFunc(GL_LEQUAL);
		glUniform1i(cubeSampler, 0);
		glUniform1f(skyAmountUniform, skyAmount);
		glUniform3fv(skyColourUniform, 1, &skyColour[0]);
		//Remove translation from camera
		glm::mat4 view = glm::mat4(glm::mat3(c->getView()));
		glm::mat4 proj = c->getProjection();
		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projUniform, 1, GL_FALSE, &proj[0][0]);
		glBindVertexArray(vertexArray);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
	}
}

void Scene::updateLights() {
	GLuint program = meshShader.getProgram();
	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "ambient"), 1, &ambientLight[0]);
	//Update directional light
	if (dirLight) {
		glm::vec3 dir =glm::vec3(dirLight->getGlobalMatrix() * glm::vec4(dirLight->direction, 1.0f));
		glUniform3fv(glGetUniformLocation(program, "dirLight.direction"), 1, &dir[0]);
		glUniform3fv(glGetUniformLocation(program, "dirLight.colour"), 1, &dirLight->colour[0]);
	}
	glUniform1i(glGetUniformLocation(program, "numDirLights"), dirLight ? 1 : 0);
	//Update point lights
	int i = 0;
	for (PointLight* p : pointLights) {
		glm::mat4 mat = p->getGlobalMatrix();
		glUniform3fv(glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].position").c_str()), 1, &mat[3][0]);
		glUniform3fv(glGetUniformLocation(program, ("pointLights[" + std::to_string(i) + "].colour").c_str()), 1, &p->colour[0]);
		glUniform1f(glGetUniformLocation(program,  ("pointLights[" + std::to_string(i) + "].quadratic").c_str()), p->quadratic);
		glUniform1f(glGetUniformLocation(program,  ("pointLights[" + std::to_string(i) + "].linear").c_str()), p->linear);
		glUniform1f(glGetUniformLocation(program,  ("pointLights[" + std::to_string(i) + "].constant").c_str()), p->constant);
		i++;
	}
	glUniform1i(glGetUniformLocation(program, "numPointLights"), pointLights.size());
	//Update spotlights
	i = 0;
	for (SpotLight* s : spotLights) {
		glm::mat4 mat = s->getGlobalMatrix();
		glm::vec3 dir = glm::vec3(glm::mat3(mat) * s->direction);
		glUniform3fv(glGetUniformLocation(program, ("spotLights[" + std::to_string(i) + "].position").c_str()), 1, &mat[3][0]);
		glUniform3fv(glGetUniformLocation(program, ("spotLights[" + std::to_string(i) + "].direction").c_str()), 1, &dir[0]);
		glUniform3fv(glGetUniformLocation(program, ("spotLights[" + std::to_string(i) + "].colour").c_str()), 1, &s->colour[0]);
		glUniform1f(glGetUniformLocation(program,  ("spotLights[" + std::to_string(i) + "].quadratic").c_str()), s->quadratic);
		glUniform1f(glGetUniformLocation(program,  ("spotLights[" + std::to_string(i) + "].linear").c_str()), s->linear);
		glUniform1f(glGetUniformLocation(program,  ("spotLights[" + std::to_string(i) + "].constant").c_str()), s->constant);
		glUniform1f(glGetUniformLocation(program,  ("spotLights[" + std::to_string(i) + "].cutOff").c_str()), s->cutOff);
		glUniform1f(glGetUniformLocation(program,  ("spotLights[" + std::to_string(i) + "].outerCutOff").c_str()), s->outerCutOff);
		i++;
	}
	glUniform1i(glGetUniformLocation(program, "numSpotLights"), spotLights.size());
	glUseProgram(0);
}

DirectionalLight* Scene::getDirectionalLight() {
	return dirLight;
}
