#include "Game.h"

#include <iostream>

#include "../renderer/Cube.h"

#define SKYBOX_FOLDER "skybox_testing"

void loadAssets(Game* game) {
	std::string folder(SKYBOX_FOLDER);
	//Skybox
	std::cout << "Loading skybox..." << std::endl;
	game->lowLodScene->loadSkybox("assets/" + folder + "/posX.png", "assets/" + folder + "/negX.png",
		"assets/" + folder + "/posY.png", "assets/" + folder + "/negY.png",
		"assets/" + folder + "/posZ.png", "assets/" + folder + "/negZ.png");
	//Player
	std::cout << "Loading models..." << std::endl;
	game->player = new Player();
	game->player->setGame(game);
	game->worldPos = glm::vec3(35000.0f, 0.0f, 0.0f);

	std::cout << "All models loaded" << std::endl;
}

void generateTerrain(Game* game) {
	//Home planet
	std::cout << "Generating terrain..." << std::endl;
	game->homeWorld = new Planet();
	game->homeWorld->planetScale = 30000.0f;
	game->homeWorld->lowLodScale = game->lowLodScale;
	float lod[] = {2000.0f, 4000.0f, 5000.0f, 20000.0f };
	game->homeWorld->setLODS(lod);
	game->homeWorld->generateTerrain();
	std::cout << "Terrain generated" << std::endl;
}

Game::Game() {
	scene = new Scene();
	lowLodScene = new Scene();
	transformedSpace = new SceneObject();
	transformedSpace->setParent(scene);
	//Asset loading
	loadAssets(this);
	//Lighting
	std::cout << "Loading lighting..." << std::endl;
	scene->ambientLight = glm::vec3(0.2f);
	sunLight = new DirectionalLight();
	sunLight->colour = glm::vec3(0.6f, 0.6f, 0.6f);
	sunLight->direction = glm::vec3(0.0f, -1.0f, 0.0f);
	sunLight->setParent(scene);
	//Terrain
	generateTerrain(this);
	//Secondary, high distance camera
	lowLodCam = new Camera();
	lowLodCam->setNear(0.1f);
	lowLodCam->setFar(1000.0f);
	lowLodCam->setParent(lowLodScene);
	lowSunLight = new DirectionalLight();
	lowSunLight->colour = glm::vec3(0.6f, 0.6f, 0.6f);
	lowSunLight->direction = glm::vec3(0.0f, -1.0f, 0.0f);
	lowSunLight->setParent(lowLodCam);
	lowLodScene->ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
}


Game::~Game() {
	if (scene) {
		delete scene;
	}
	if (transformedSpace) {
		delete transformedSpace;
	}
	if (lowLodScene) {
		delete lowLodScene;
	}
	if (player) {
		delete player;
	}
	if (sunLight) {
		delete sunLight;
	}
	if (lowLodCam) {
		delete lowLodCam;
	}
}

void Game::keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	player->keyEvent(window, key, scancode, action, mods);
}

void Game::update(double dt) {
	if (player) {
		player->update(dt);
	}
	homeWorld->updateVisible(transformedSpace, lowLodScene, worldPos);
}

void Game::draw() {
	if (!player) { return; }
	Camera* cam = player->getActiveCamera();
	if (!cam) { return; }
	lowLodCam->setLocalMatrix(cam->getGlobalMatrix());
	lowLodCam->setPosition(worldPos * lowLodScale);
	lowLodCam->render();
	transformedSpace->setPosition(-worldPos);
	cam->render();
}
