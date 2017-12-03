#pragma once
#include "../renderer/Model.h"
#include "../renderer/Camera.h"

enum class CurrentCamera {COCKPIT, ORBITAL};

class Game;

class Player {
public:
	Player();
	~Player();
	void update(double dt);
	void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	void setGame(Game* g);
	Model* getShip() {
		return ship;
	};
	Camera* getActiveCamera();
private:
	void moveCockpitCamera();
	void rotateShip();
	void moveShip(float dt);
	Game* game;
	Model* ship;
	Camera* cockpit;
	Camera* orbital;
	float cockpitYaw = 0.0f;
	float cockpitPitch = 0.0f;
	float shipSpeed = 10.0f;
	bool boostSpeed = false;
	CurrentCamera activeCam;
};

