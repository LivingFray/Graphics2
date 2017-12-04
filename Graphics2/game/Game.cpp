#include "Game.h"

#include <iostream>

#include "../renderer/Cube.h"

#define SKYBOX_FOLDER "skybox"

#define ATMOS_MIN 2000.0f
#define ATMOS_MAX 6000.0f

#define UPDATE_DIST 10000.0f

void loadAssets(Game* game) {
	std::string folder(SKYBOX_FOLDER);
	//Skybox
	std::cout << "Loading skybox..." << std::endl;
	game->lowLodScene->loadSkybox("assets/" + folder + "/posX.png", "assets/" + folder + "/negX.png",
		"assets/" + folder + "/posY.png", "assets/" + folder + "/negY.png",
		"assets/" + folder + "/posZ.png", "assets/" + folder + "/negZ.png");
	game->secondLowLodScene->loadSkybox("assets/" + folder + "/posX.png", "assets/" + folder + "/negX.png",
		"assets/" + folder + "/posY.png", "assets/" + folder + "/negY.png",
		"assets/" + folder + "/posZ.png", "assets/" + folder + "/negZ.png");
	//Player
	std::cout << "Loading models..." << std::endl;
	game->player = new Player();
	game->player->setGame(game);
	game->player->getShip()->createOctrees(2);
	game->worldPos = glm::vec3(38000.0f, 0.0f, 0.0f);
	//Portal
	game->portal = new Portal();
	game->portal->initPortalMap();
	game->portal->portalScale = game->lowLodScale;
	game->portal->portalSurface = new Model();
	game->portal->portalSurface->loadModel("assets/portal/portal.obj");
	game->portal->setRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.0f, 0.0f)));
	game->portal->setPosition(glm::vec3(38000.0f, 0.0f, -10.0f));
	game->portal->exitPortal = new SceneObject();
	game->portal->exitPortal->setParent(game->secondLowLodScene);
	game->portal->exitPortal->setPosition(glm::vec3(game->lowLodScale * 38000.0f, 0.0f, 0.0f));
	game->portal->exitPortal->setRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), glm::half_pi<float>(), 0.0f)));
	game->portal->setParent(game->transformedSpace);
	game->portal->renderView = new Camera();
	game->portal->renderView->setParent(game->portal->exitPortal);
	game->portal->renderView->setNear(0.1f);
	game->portal->renderView->setFar(1000.0f);
	//Gate
	Model* gate = new Model();
	gate->loadModel("assets/portal/gate.obj");
	gate->setParent(game->transformedSpace);
	gate->setPosition(glm::vec3(38000.0f, 0.0f, -10.0f));
	gate->setRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.0f, 0.0f)));
	std::cout << "All assets loaded" << std::endl;

}

void generateTerrain(Game* game) {
	//Home planet
	std::cout << "Generating terrain..." << std::endl;
	game->homeWorld = new Planet();
	game->homeWorld->planetScale = 30000.0f;
	game->homeWorld->lowLodScale = game->lowLodScale;
	float lod[] = {1000.0f, 4000.0f, 8000.0f, 64000.0f };
	game->homeWorld->setLODS(lod);
	//Make world look gaian
	game->homeWorld->setLandTexture(OpenGLSetup::loadImage("assets/terrain/grass.png"));
	game->homeWorld->setSeaTexture(OpenGLSetup::loadImage("assets/terrain/water.png"));
	game->homeWorld->setSeaSpecular(OpenGLSetup::loadImage("assets/terrain/white.png"));
	game->homeWorld->setRockTexture(OpenGLSetup::loadImage("assets/terrain/rock.png"));
	//Generate terrain
	game->homeWorld->generateTerrain(0);
	std::cout << "Homeworld generated" << std::endl;
	//Other planet
	game->otherWorld = new Planet();
	game->otherWorld->planetScale = 15000.0f;
	game->otherWorld->lowLodScale = game->lowLodScale;
	float lod2[] = { 1000.0f, 4000.0f, 8000.0f, 64000.0f };
	game->otherWorld->setLODS(lod2);
	//Make world look martian
	game->otherWorld->setLandTexture(OpenGLSetup::loadImage("assets/terrain/marsRock.png"));
	game->otherWorld->setSeaTexture(OpenGLSetup::loadImage("assets/terrain/marsRock.png"));
	game->otherWorld->setRockTexture(OpenGLSetup::loadImage("assets/terrain/marsRock.png"));
	//Adjust terrain parameters
	game->otherWorld->setMinY(-0.10f);
	game->otherWorld->setMaxY(0.10f);
	game->otherWorld->setSeaHeight(0.0f);
	game->otherWorld->setRoughness(0.5f);
	game->otherWorld->setNodeExp(6);
	//Generate terrain
	game->otherWorld->generateTerrain(0);
	SceneObject h;
	std::unordered_set<Mesh*> hp;
	game->otherWorld->updateVisible(&h, game->secondLowLodScene, game->portal->exitPortal->getPosition() / game->lowLodScale, hp);
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
	sunLight = new DirectionalLight();
	sunLight->colour = glm::vec3(0.8f, 0.8f, 0.8f);
	sunLight->direction = glm::vec3(0.0f, -1.0f, 0.0f);
	sunLight->setParent(secondLowLodScene);
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
	//Set correct scene
	inFirstScene = true;
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
	Planet* p = inFirstScene ? homeWorld : otherWorld;
	//TODO: Change strength of light depending on occlusion of planet
	glm::vec3 oldPos = worldPos;
	if (player) {
		player->update(dt);
		lowLodScene->skyAmount = 1.0f - glm::clamp((glm::length(worldPos) - p->planetScale - ATMOS_MIN)/(ATMOS_MAX - ATMOS_MIN), 0.0f, 1.0f);
	}
	//Handle movement
	if (forceVisualUpdate || oldPos != worldPos) {
		p->updateVisible(transformedSpace, lowLodScene, worldPos, highPoly);
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
