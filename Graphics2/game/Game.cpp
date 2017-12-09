#include "Game.h"
#include <iostream>
#include "../renderer/Cube.h"
#include "../renderer/glm/gtc/matrix_transform.hpp"


#define SKYBOX_FOLDER "skybox"

#define ATMOS_MIN 2000.0f
#define ATMOS_MAX 6000.0f

#define OCTDEPTH 4

#define DIAL_TIME 0.5f

#define DIAL_DISTANCE 50.0f

#define DIAL_ROTATE 2.0f

#define DIAL_STAGE_PORTAL 9
#define DIAL_STAGE_MOVE 10
#define DIAL_STAGE_STOP_MOVE 11

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
	game->player->getShip()->createOctrees(0);
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
	game->portal->renderView = new Camera();
	game->portal->renderView->setParent(game->portal->exitPortal);
	game->portal->renderView->setNear(0.1f);
	game->portal->renderView->setFar(1000.0f);
	//Gate
	game->gate = new Model();
	game->gate->loadModel("assets/portal/gate.obj");
	game->gate->setParent(game->transformedSpace);
	game->gate->setPosition(glm::vec3(38000.0f, 0.0f, -10.0f));
	game->gate->setRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0.0f, 0.0f)));
	std::cout << "All assets loaded" << std::endl;

}

void generateTerrain(Game* game) {
	//Home planet
	std::cout << "Generating terrain..." << std::endl;
	game->homeWorld = new Planet();
	game->homeWorld->planetScale = 30000.0f;
	game->homeWorld->lowLodScale = game->lowLodScale;
	float lod[] = { 2000.0f, 4000.0f, 8000.0f, 16000.0f };
	game->homeWorld->setLODS(lod);
	game->homeWorld->lowLodHeight = 500.0f;
	//Make world look gaian
	game->homeWorld->setLandTexture(OpenGLSetup::loadImage("assets/terrain/grass.png"));
	game->homeWorld->setSeaTexture(OpenGLSetup::loadImage("assets/terrain/water.png"));
	game->homeWorld->setSeaSpecular(OpenGLSetup::loadImage("assets/terrain/white.png"));
	game->homeWorld->setRockTexture(OpenGLSetup::loadImage("assets/terrain/rock.png"));
	//Generate terrain
	game->homeWorld->generateTerrain(OCTDEPTH);
	std::cout << "Homeworld generated" << std::endl;
	//Other planet
	game->otherWorld = new Planet();
	game->otherWorld->planetScale = 15000.0f;
	game->otherWorld->lowLodScale = game->lowLodScale;
	float lod2[] = { 2000.0f, 4000.0f, 8000.0f, 16000.0f };
	game->otherWorld->setLODS(lod2);
	//Make world look martian
	game->otherWorld->setLandTexture(OpenGLSetup::loadImage("assets/terrain/marsRock.png"));
	game->otherWorld->setSeaTexture(OpenGLSetup::loadImage("assets/terrain/marsRock.png"));
	game->otherWorld->setRockTexture(OpenGLSetup::loadImage("assets/terrain/marsRock.png"));
	//Adjust terrain parameters
	game->otherWorld->setMinY(-0.05f);
	game->otherWorld->setMaxY(0.05f);
	game->otherWorld->setSeaHeight(0.0f);
	game->otherWorld->setRoughness(0.5f);
	game->otherWorld->setNodeExp(6);
	//Generate terrain
	game->otherWorld->generateTerrain(OCTDEPTH);
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
	float radius = 2.7f;
	float raise = -0.5f;
	for (int i = 0; i < 8; i++) {
		PointLight* p = new PointLight();
		p->colour = glm::vec3(1.0f, 0.0f, 0.0f);
		p->constant = 1.0;
		p->linear = 0.7f;
		p->quadratic = 1.8f;
		float ang = i * glm::two_pi<float>() / 8;
		p->setPosition(glm::vec3(sinf(ang) * radius, raise, cosf(ang) * radius));
		p->setParent(gate);
		gateLights[i] = p;
	}
	//Terrain
	generateTerrain(this);
	//Secondary, high distance camera
	lowLodCam = new Camera();
	lowLodCam->setNear(0.1f);
	lowLodCam->setFar(1000.0f);
	lowLodCam->setParent(lowLodScene);
	DirectionalLight* lowSunLight = new DirectionalLight();
	lowSunLight->colour = glm::vec3(0.8f, 0.8f, 0.8f);
	lowSunLight->direction = glm::vec3(0.0f, -1.0f, 0.0f);
	lowSunLight->setParent(lowLodScene);
	//Update visible geometry
	forceVisualUpdate = true;
	//Set correct scene
	inFirstScene = true;
	//Set animation states
	dialTime = 0;
	dialState = 0;
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
	for (int i = 0; i < 8; i++) {
		if (gateLights[i]) {
			delete gateLights[i];
		}
	}
}

void Game::keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	player->keyEvent(window, key, scancode, action, mods);
}

void Game::update(double dt) {
	//TODO: Change strength of light depending on occlusion of planet
	glm::vec3 oldPos = worldPos;
	glm::quat oldRot = player->getShip()->getRotation();
	player->update(dt);
	if (dialState > 0 && dialState < 12) {
		if (dialState == 1 && rotationProgress < 1.0f) {
			rotationProgress += dt / DIAL_ROTATE;
			rotationProgress = glm::min(1.0f, rotationProgress);
			gate->setRotation(glm::slerp(startGateRotation, requiredGateRotation, rotationProgress));
			player->getShip()->setRotation(glm::slerp(startShipRotation, requiredShipRotation, rotationProgress));
		} else {
			dialTime += dt;
			if (dialTime > DIAL_TIME) {
				dialTime -= DIAL_TIME;
				int old = dialState;
				if (dialState != DIAL_STAGE_MOVE) {
					dialState++;
				}
				if (dialState < 10) {
					gateLights[dialState - 2]->colour = glm::vec3(0.0f, 1.0f, 0.0f);
				}
				if (dialState == DIAL_STAGE_PORTAL && old != DIAL_STAGE_PORTAL) {
					portal->setParent(transformedSpace);
					portal->setRotation(gate->getRotation());
				}
			}
			if (dialState == 10) {
				worldPos += player->getShip()->getFront() * player->slowSpeed * static_cast<float>(dt);
				//Check if camera has crossed threshold of portal
				glm::vec3 front = player->getActiveCamera()->getGlobalPosition() + player->getShip()->getFront() * 0.5f;
				front -= gate->getGlobalPosition();
				glm::vec3 back = player->getActiveCamera()->getGlobalPosition() + player->getShip()->getFront() * -100.0f;
				back -= gate->getGlobalPosition();
				if (glm::dot(front, back) < 0.0f) {
					dialState = 11;
					dialTime = 0.0f;
					enterGate();
				}
			}else if (dialState == 11) {
				worldPos += player->getShip()->getFront() * player->slowSpeed * static_cast<float>(dt);
			}
			if (dialState == 12) {
				//Restore control of ship
				player->canMove = true;
				player->forceCockpit = false;
			}
		}
	} else if(dialState == 0) {
		glm::vec3 p = gate->getGlobalPosition();
		player->canDialGate = glm::dot(p, p) < DIAL_DISTANCE * DIAL_DISTANCE;
	}
	Planet* p = inFirstScene ? homeWorld : otherWorld;
	lowLodScene->skyAmount = 1.0f - glm::clamp((glm::length(worldPos) - p->planetScale - ATMOS_MIN) / (ATMOS_MAX - ATMOS_MIN), 0.0f, 1.0f);
	//Handle movement
	if (forceVisualUpdate || oldPos != worldPos) {
		p->updateVisible(transformedSpace, lowLodScene, worldPos, highPoly);
		forceVisualUpdate = false;
		transformedSpace->setPosition(-worldPos);
	}
	if (oldPos != worldPos || oldRot != player->getShip()->getRotation()) {
		for (Mesh* m : highPoly) {
			if (player->getShip()->collides(m->collisionTree, m->getGlobalMatrix())) {
				worldPos = oldPos;
				transformedSpace->setPosition(-worldPos);
				player->getShip()->setRotation(oldRot);
				break;
			}
		}
	}
}

void Game::draw() {
	if (!player) { return; }
	Camera* cam = player->getActiveCamera();
	if (!cam) { return; }
	lowLodCam->setLocalMatrix(cam->getGlobalMatrix());
	lowLodCam->setPosition(worldPos * lowLodScale);
	lowLodCam->render();
	cam->render();
}

void Game::dialGate() {
	if (dialState == 0) {
		dialState = 1;
		rotationProgress = 0.0f;
		requiredGateRotation = glm::conjugate(glm::quat(glm::lookAt(gate->getGlobalPosition(),
			player->getShip()->getGlobalPosition(),
			glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::quat(glm::vec3(-glm::half_pi<float>(), glm::pi<float>(), 0.0f));
		requiredShipRotation = glm::conjugate(glm::quat(glm::lookAt(player->getShip()->getGlobalPosition(),
			gate->getGlobalPosition(), glm::vec3(0.0f, 1.0f, 0.0f))));
		startGateRotation = gate->getRotation();
		startShipRotation = player->getShip()->getRotation();
	}
}

void Game::enterGate() {
	//Move player to correct location
	float dif = glm::length(player->getShip()->getGlobalPosition() - gate->getGlobalPosition());
	player->getShip()->setRotation(-portal->exitPortal->getRotation() * glm::vec3(0.0f, 0.0f, glm::half_pi<float>()));
	worldPos = portal->exitPortal->getGlobalPosition() / lowLodScale - player->getShip()->getFront() * dif;
	//Rotate gate
	gate->setRotation(-portal->exitPortal->getRotation());
	gate->setPosition(portal->exitPortal->getGlobalPosition() / lowLodScale);
	//Clean up gate
	SceneObject* null = NULL;
	portal->portalSurface->setParent(null);
	portal->exitPortal->setParent(null);
	portal->setParent(null);
	delete portal->portalSurface;
	delete portal->exitPortal;
	delete portal;
	//Reset lights
	for (int i = 0; i < 8; i++) {
		//Turn off lights
		gateLights[i]->colour = glm::vec3(1.0f, 0.0f, 0.0f);
		//Move lights to act as if coming out of front of gate
		glm::vec3 pos = gateLights[i]->getPosition();
		pos.y *= -1;
		gateLights[i]->setPosition(pos);
	}
	//Change scene
	inFirstScene = false;
	homeWorld->hide();
	otherWorld->hide();
	forceVisualUpdate = true;
	scene->skyColour = glm::vec3(255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f);
	lowLodScene->skyColour = glm::vec3(255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f);
}
