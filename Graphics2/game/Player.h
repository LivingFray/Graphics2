#pragma once
#include "../renderer/Model.h"
#include "../renderer/Camera.h"
#include "../renderer/SpotLight.h"

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
	bool canMove;
	bool forceCockpit;
	bool canDialGate;
	bool gateDialed;
	const float speeds[5] = {10.0f, 20.0f, 50.0f, 100.0f, 1000.0f};
	float collideWarning = 0.0f;
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
	SpotLight* dialReady;
	Mesh* gps;
	Mesh* speedometer;
	Mesh* prox;
	GLuint speedImgs[6];
	GLuint gpsImgs[4];
	GLuint proxImgs[6];
};

