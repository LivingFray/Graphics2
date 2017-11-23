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
	cockpit = new Camera();
	cockpit->setPosition(glm::vec3(0.0f, 0.0f, -1.5f));
	cockpit->setParent(ship);
	cockpit->setNear(0.1f);
	cockpit->setFar(12000.0f);
	cockpit->clearOnDraw = false;
	orbital = new Camera();
	orbital->setPosition(glm::vec3(0.0f, 1.5f, 5.0f));
	orbital->setRotation(glm::quat(glm::vec3(-glm::pi<float>() / 16.0f, 0.0f, 0.0f)));
	orbital->setParent(ship);
	orbital->setNear(0.1f);
	orbital->setFar(12000.0f);
	orbital->clearOnDraw = false;
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
}

void Player::update(double dt) {
	if (activeCam == CurrentCamera::COCKPIT) {
		moveCockpitCamera();
	} else if (activeCam == CurrentCamera::ORBITAL) {
		rotateShip();
	}
	moveShip(static_cast<float>(dt));
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

	if (cockpitYaw > glm::two_pi<double>()) {
		cockpitYaw -= glm::two_pi<double>();
	}
	if (cockpitYaw < 0.0) {
		cockpitYaw += glm::two_pi<double>();
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
