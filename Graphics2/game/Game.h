#pragma once
#include "../renderer/Scene.h"
#include "Player.h"
#include "../terrain/Planet.h"
#include <unordered_set>

class Game {
	friend void generateTerrain(Game* game);
	friend void loadAssets(Game* game);
public:
	Game();
	~Game();
	Scene* scene;
	Player* player;
	void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	void update(double dt);
	void draw();
	glm::vec3 worldPos;
	const float lowLodScale = 1.0f/1000.0f;
private:
	DirectionalLight* sunLight;
	DirectionalLight* lowSunLight;
	Planet* homeWorld;
	Camera* lowLodCam;
	Scene* lowLodScene;
	SceneObject* transformedSpace;
	bool forceVisualUpdate;
	std::unordered_set<Mesh*> highPoly;
};

