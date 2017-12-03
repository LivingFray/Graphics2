#include "Game.h"

#include <iostream>

#include "../renderer/Cube.h"

#define SKYBOX_FOLDER "skybox_testing"

#define ATMOS_MIN 2000.0f
#define ATMOS_MAX 6000.0f

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
	game->player->getShip()->createOctrees(2);
	game->worldPos = glm::vec3(38000.0f, 0.0f, 0.0f);

	game->portal = new Portal();
	game->portal->initPortalMap();
	game->portal->portalScale = game->lowLodScale;
	game->portal->portalSurface = new Model();
	game->portal->portalSurface->loadModel("assets/portal/portal.obj");
	game->portal->setRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.0f, 0.0f)));
	game->portal->setPosition(glm::vec3(38000.0f, 0.0f, -10.0f));
	game->portal->exitPortal = new SceneObject();
	game->portal->exitPortal->setParent(game->lowLodScene);
	game->portal->exitPortal->setPosition(glm::vec3(game->lowLodScale * 38000.0f, 0.0f, 0.0f));
	game->portal->exitPortal->setRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), glm::half_pi<float>(), 0.0f)));
	game->portal->setParent(game->transformedSpace);
	game->portal->renderView = new Camera();
	game->portal->renderView->setParent(game->portal->exitPortal);
	game->portal->renderView->setNear(0.1f);
	game->portal->renderView->setFar(1000.0f);

	Model* gate = new Model();
	gate->loadModel("assets/portal/gate.obj");
	gate->setParent(game->transformedSpace);
	gate->setPosition(glm::vec3(38000.0f, 0.0f, -10.0f));
	gate->setRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.0f, 0.0f)));
	std::cout << "All models loaded" << std::endl;

}

void generateTerrain(Game* game) {
	//Home planet
	std::cout << "Generating terrain..." << std::endl;
	game->homeWorld = new Planet();
	game->homeWorld->planetScale = 30000.0f;
	game->homeWorld->lowLodScale = game->lowLodScale;
	float lod[] = {1000.0f, 4000.0f, 8000.0f, 64000.0f };
	game->homeWorld->setLODS(lod);
	game->homeWorld->generateTerrain(0);
	std::cout << "Terrain generated" << std::endl;
}

Game::Game() {
	//First planet
	scene = new Scene();
	//Low resolution version of first planet
	lowLodScene = new Scene();
	//Space local to the ship
	transformedSpace = new SceneObject();
	transformedSpace->setParent(scene);
	//Second planet
	secondScene = new Scene();
	//Low resolution version of second planet
	secondLowLodScene = new Scene();
	//Asset loading
	loadAssets(this);
	//Lighting
	std::cout << "Loading lighting..." << std::endl;
	scene->ambientLight = glm::vec3(0.2f);
	DirectionalLight* sunLight = new DirectionalLight();
	sunLight->colour = glm::vec3(0.8f, 0.8f, 0.8f);
	sunLight->direction = glm::vec3(0.0f, -1.0f, 0.0f);
	sunLight->setParent(scene);
	//Terrain
	generateTerrain(this);
	//Secondary, high distance camera
	lowLodCam = new Camera();
	lowLodCam->setNear(0.1f);
	lowLodCam->setFar(1000.0f);
	lowLodCam->setParent(lowLodScene);
	DirectionalLight* lowSunLight = new DirectionalLight();
	lowSunLight->colour = glm::vec3(0.8f, 0.8f, 0.68);
	lowSunLight->direction = glm::vec3(0.0f, -1.0f, 0.0f);
	lowSunLight->setParent(lowLodScene);
	//Update visible geometry
	forceVisualUpdate = true;
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
}

void Game::keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	player->keyEvent(window, key, scancode, action, mods);
}

void Game::update(double dt) {
	//TODO: Change strength of light depending on occlusion of planet
	glm::vec3 oldPos = worldPos;
	if (player) {
		player->update(dt);
		lowLodScene->skyAmount = 1.0f - glm::clamp((glm::length(worldPos) - homeWorld->planetScale - ATMOS_MIN)/(ATMOS_MAX - ATMOS_MIN), 0.0f, 1.0f);
	}
	//Handle movement
	if (forceVisualUpdate || oldPos != worldPos) {
		homeWorld->updateVisible(transformedSpace, lowLodScene, worldPos, highPoly);
		//*
		for(Mesh* m: highPoly) {
			if (player->getShip()->collides(m->collisionTree, m->getGlobalMatrix())) {
				worldPos = oldPos;
				break;
			}
		}
		//*/
		forceVisualUpdate = false;
	}
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
