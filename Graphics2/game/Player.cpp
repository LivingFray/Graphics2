#include "Player.h"
#include "Game.h"

//Control scheme to use (SUPERIOR_CONTROLS - Left handed IJKL, GAMER_CONTROLS - WASD, Anything else - Arrow keys)
#define SUPERIOR_CONTROLS

#define MOUSE_SPEED 0.005
#define ROLL_SPEED 1.0

#ifdef SUPERIOR_CONTROLS

#define MOVE_FORWARD GLFW_KEY_I
#define MOVE_BACKWARD GLFW_KEY_K
#define MOVE_LEFT GLFW_KEY_J
#define MOVE_RIGHT GLFW_KEY_L
#define ROLL_LEFT GLFW_KEY_U
#define ROLL_RIGHT GLFW_KEY_O

#else

#ifdef GAMER_CONTROLS

#define MOVE_FORWARD GLFW_KEY_W
#define MOVE_BACKWARD GLFW_KEY_S
#define MOVE_LEFT GLFW_KEY_A
#define MOVE_RIGHT GLFW_KEY_D
#define ROLL_LEFT GLFW_KEY_Q
#define ROLL_RIGHT GLFW_KEY_E

#else

#define MOVE_FORWARD GLFW_KEY_UP
#define MOVE_BACKWARD GLFW_KEY_DOWN
#define MOVE_LEFT GLFW_KEY_LEFT
#define MOVE_RIGHT GLFW_KEY_RIGHT
#define ROLL_LEFT GLFW_KEY_RIGHT_CONTROL
#define ROLL_RIGHT GLFW_KEY_RIGHT_SHIFT

#endif
#endif

Player::Player() {
	ship = new Model();
	ship->loadModel("assets/ship/ship.obj");
	for (Mesh* m : ship->meshes) {
		if (m->getName() == "GPS_Screen") {
			gps = m;
		} else if (m->getName() == "Speed_Screen") {
			speedometer = m;
		}
	}
	GLuint screenBacking = OpenGLSetup::loadImage("assets/ship/screen.png");
	GLuint screenSpec = OpenGLSetup::loadImage("assets/ship/screenSpec.png");
	gpsImgs[0] = OpenGLSetup::loadImage("assets/ship/screenGPSUp.png");
	gpsImgs[1] = OpenGLSetup::loadImage("assets/ship/screenGPSDown.png");
	gpsImgs[2] = OpenGLSetup::loadImage("assets/ship/screenGPSLeft.png");
	gpsImgs[3] = OpenGLSetup::loadImage("assets/ship/screenGPSRight.png");
	gps->setDiffuse(screenBacking);
	speedometer->setDiffuse(screenBacking);
	gps->setSpecular(screenSpec);
	speedometer->setSpecular(screenSpec);
	gps->useNormalTexture = false;
	speedometer->useNormalTexture = false;
	gps->setEmission(OpenGLSetup::loadImage("assets/ship/screenGPSUp.png"));
	cockpit = new Camera();
	cockpit->setPosition(glm::vec3(0.0f, 0.0f, -1.0f));
	cockpit->setParent(ship);
	cockpit->setNear(0.1f);
	cockpit->setFar(4000.0f);
	cockpit->clearOnDraw = false;
	orbital = new Camera();
	orbital->setPosition(glm::vec3(0.0f, 1.5f, 5.0f));
	orbital->setRotation(glm::quat(glm::vec3(-glm::pi<float>() / 16.0f, 0.0f, 0.0f)));
	orbital->setParent(ship);
	orbital->setNear(0.1f);
	orbital->setFar(4000.0f);
	orbital->clearOnDraw = false;
	orbital->orthosize = 50.0f;
	forceCockpit = false;
	canMove = true;
	canDialGate = false;
	dialReady = new SpotLight();
	dialReady->constant = 1.0f;
	dialReady->linear = 0.35f;
	dialReady->quadratic = 0.44f;
	dialReady->direction = glm::normalize(glm::vec3(0.0f, -1.0f, -0.5f));
	dialReady->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	dialReady->setParent(ship);
	dialReady->cutOff = cosf(glm::two_pi<float>() / 32.0f);
	dialReady->outerCutOff = cosf(glm::two_pi<float>() / 16.0f);
	dialReady->colour = glm::vec3(0.0f, 0.0f, 0.0f);
}


Player::~Player() {
	if (ship) {
		delete ship;
	}
	if (cockpit) {
		delete cockpit;
	}
	if (orbital) {
		delete orbital;
	}
	if (dialReady) {
		delete dialReady;
	}
}

void Player::update(double dt) {
	if (forceCockpit) {
		activeCam = CurrentCamera::COCKPIT;
	}
	if (activeCam == CurrentCamera::COCKPIT) {
		if (glfwGetMouseButton(OpenGLSetup::window, GLFW_MOUSE_BUTTON_2)) {
			moveCockpitCamera();
		} else if (canMove) {
			cockpitYaw = 0.0f;
			cockpitPitch = 0.0f;
			cockpit->setRotation(glm::quat(glm::vec3(-cockpitPitch, -cockpitYaw, 0.0)));
			rotateShip();
		}
		//Update GPS
		//Convert gate to ship space
		glm::vec3 gatePos = glm::vec3(ship->getGlobalMatrix() * glm::vec4(game->gate->getGlobalPosition(), 1.0f));
		//Get angle in XZ plane
		float xzAng = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(gatePos.x, 0.0f, gatePos.z)));
		//Get angle in YZ plane
		float yzAng = glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(0.0f, gatePos.y, gatePos.z)));
		//Largest angle determines plane
		//Sign of angle determines direction
		if(abs(xzAng) > abs(yzAng)) {
			if (xzAng > 0.0f) {
				gps->setEmission(gpsImgs[2]);
			} else {
				gps->setEmission(gpsImgs[3]);
			}
		} else {
			if (yzAng > 0.0f) {
				gps->setEmission(gpsImgs[1]);
			} else {
				gps->setEmission(gpsImgs[0]);
			}
		}
	} else if (activeCam == CurrentCamera::ORBITAL && canMove) {
		rotateShip();
	}
	if (canMove) {
		moveShip(static_cast<float>(dt));
	}
	dialReady->colour = canDialGate ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f);
}

void Player::keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		//Implemented this way to allow for more cameras to be added
		switch (activeCam) {
		case CurrentCamera::COCKPIT:
			activeCam = CurrentCamera::ORBITAL;
			break;
		case CurrentCamera::ORBITAL:
			activeCam = CurrentCamera::COCKPIT;
			break;
		default:
			break;
		}
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		boostSpeed = !boostSpeed;
		if (boostSpeed) {
			shipSpeed *= 100.0f;
		} else {
			shipSpeed /= 100.0f;
		}
	}
	//Only allow dialing if in cockpit mode for "cinematic reasons" (definitely not to hide rendering issues with portal)
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && canDialGate && canMove && activeCam == CurrentCamera::COCKPIT) {
		game->dialGate();
		canMove = false;
		canDialGate = false;
		forceCockpit = true;
	}
}

void Player::setGame(Game* g) {
	game = g;
	if (game) {
		ship->setParent(game->scene);
	}
}

Camera* Player::getActiveCamera() {
	switch (activeCam) {
	case CurrentCamera::COCKPIT:
		return cockpit;
	case CurrentCamera::ORBITAL:
		return orbital;
	default:
		return NULL;
	}
}

void Player::moveCockpitCamera() {
	double mx, my;
	int w, h;
	glfwGetCursorPos(OpenGLSetup::window, &mx, &my);
	glfwGetWindowSize(OpenGLSetup::window, &w, &h);
	mx -= static_cast<double>(w) / 2.0;
	my -= static_cast<double>(h) / 2.0;
	glfwSetCursorPos(OpenGLSetup::window,
		static_cast<double>(w) / 2.0,
		static_cast<double>(h) / 2.0
	);

	cockpitYaw += mx * MOUSE_SPEED;
	cockpitPitch += my * MOUSE_SPEED;

	if (cockpitYaw > glm::half_pi<double>()) {
		cockpitYaw = glm::half_pi<double>();
	}
	if (cockpitYaw < -glm::half_pi<double>()) {
		cockpitYaw = -glm::half_pi<double>();
	}
	if (cockpitPitch > glm::half_pi<double>()) {
		cockpitPitch = glm::half_pi<double>();
	}
	if (cockpitPitch < -glm::half_pi<double>()) {
		cockpitPitch = -glm::half_pi<double>();
	}
	cockpit->setRotation(glm::quat(glm::vec3(-cockpitPitch, -cockpitYaw, 0.0)));
}

void Player::rotateShip() {
	double mx, my;
	int w, h;
	glfwGetCursorPos(OpenGLSetup::window, &mx, &my);
	glfwGetWindowSize(OpenGLSetup::window, &w, &h);
	mx -= static_cast<double>(w) / 2.0;
	my -= static_cast<double>(h) / 2.0;
	glfwSetCursorPos(OpenGLSetup::window,
		static_cast<double>(w) / 2.0,
		static_cast<double>(h) / 2.0
	);
	ship->setRotation(ship->getRotation() * glm::quat(glm::vec3(-my * MOUSE_SPEED, -mx * MOUSE_SPEED, 0.0f)));
}

void Player::moveShip(float dt) {
	//Prevent Floating point errors by moving the world around the ship
	//"The engines don't move the ship at all.
	//The ship stays where it is and the engines move the universe around it"
	//~C. Farnsworth, Futurama
	glm::vec3 front = ship->getFront();
	glm::vec3 right = ship->getRight();
	if (glfwGetKey(OpenGLSetup::window, MOVE_FORWARD)) {
		game->worldPos += front * shipSpeed * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, MOVE_BACKWARD)) {
		game->worldPos -= front * shipSpeed * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, MOVE_LEFT)) {
		game->worldPos -= right * shipSpeed * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, MOVE_RIGHT)) {
		game->worldPos += right * shipSpeed * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, ROLL_LEFT)) {
		ship->setRotation(ship->getRotation() * glm::quat(glm::vec3(0.0f, 0.0f, ROLL_SPEED * dt)));
	}
	if (glfwGetKey(OpenGLSetup::window, ROLL_RIGHT)) {
		ship->setRotation(ship->getRotation() * glm::quat(glm::vec3(0.0f, 0.0f, -ROLL_SPEED * dt)));
	}
}
