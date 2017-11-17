#include "Game.h"

#include "../renderer/Cube.h"

Game::Game() {
	scene = new Scene();
	scene->loadSkybox("assets/skybox/posX.png", "assets/skybox/negX.png",
		"assets/skybox/posY.png", "assets/skybox/negY.png",
		"assets/skybox/posZ.png", "assets/skybox/negZ.png");
	//Lighting
	scene->ambientLight = glm::vec3(0.2f);
	sunLight = new DirectionalLight();
	sunLight->colour = glm::vec3(0.6f, 0.6f, 0.6f);
	sunLight->direction = glm::vec3(0.0f, -1.0f, 0.0f);
	sunLight->setParent(scene);
	player = new Player();
	player->setGame(this);

	Cube* test = new Cube();
	test->setParent(scene);
}


Game::~Game() {
	if (scene) {
		delete scene;
	}
	if (player) {
		delete player;
	}
	if (sunLight) {
		delete sunLight;
	}
}

void Game::keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	player->keyEvent(window, key, scancode, action, mods);
}

void Game::update(double dt) {
	if (player) {
		player->update(dt);
	}
}

void Game::draw() {
	if (!player) { return; }
	Camera* cam = player->getActiveCamera();
	if (cam) {
		cam->render();
	}
}
