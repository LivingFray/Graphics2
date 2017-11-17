#pragma once
#include "../renderer/Scene.h"
#include "Player.h"
class Game {
public:
	Game();
	~Game();
	Scene* scene;
	Player* player;
	void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	void update(double dt);
	void draw();
private:
	DirectionalLight* sunLight;
};

