#include "Scene.h"
#include <stb_image.h>


Scene::Scene() {
	//Skybox related things
	skybox = 0;
	shader = Shader("shaders/skybox.vert", "shaders/skybox.frag");
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), &vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	viewUniform = glGetUniformLocation(shader.getProgram(), "view");
	projUniform = glGetUniformLocation(shader.getProgram(), "projection");
	cubeSampler = glGetUniformLocation(shader.getProgram(), "skybox");
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
	data = stbi_load(posY.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	data = stbi_load(negY.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	data = stbi_load(posZ.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	data = stbi_load(negZ.c_str(), &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Scene::renderSkybox(Camera* c) {
	if (skybox) {
		//Set shaders
		glUseProgram(shader.getProgram());
		glDepthFunc(GL_LEQUAL);
		glUniform1i(cubeSampler, 0);
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

