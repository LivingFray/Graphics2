#include "Player.h"
#include "Game.h"

#define MOUSE_SPEED 0.005
#define CAMERA_SPEED 10.0f


Player::Player() {
	ship = new Model();
	ship->loadModel("assets/ship/ship.obj");
	cockpit = new Camera();
	cockpit->setPosition(glm::vec3(0.0f, 0.0f, -3.0f));
	cockpit->setParent(ship);
	orbital = new Camera();
	orbital->setPosition(glm::vec3(0.0f, 3.0f, 10.0f));
	orbital->setRotation(glm::quat(glm::vec3(-glm::pi<float>() / 16.0f, 0.0f, 0.0f)));
	orbital->setParent(ship);
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

	shipYaw += mx * MOUSE_SPEED;
	shipPitch += my * MOUSE_SPEED;

	if (shipYaw > glm::two_pi<double>()) {
		shipYaw -= glm::two_pi<double>();
	}
	if (shipYaw < 0.0) {
		shipYaw += glm::two_pi<double>();
	}
	if (shipPitch > glm::half_pi<double>()) {
		shipPitch = glm::half_pi<double>();
	}
	if (shipPitch < -glm::half_pi<double>()) {
		shipPitch = -glm::half_pi<double>();
	}
	ship->setRotation(glm::quat(glm::vec3(-shipPitch, -shipYaw, 0.0)));
}

void Player::moveShip(float dt) {
	glm::vec3 pos = ship->getPosition();
	glm::vec3 front = ship->getFront();
	glm::vec3 right = ship->getRight();
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_I)) {
		pos += front * CAMERA_SPEED * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_K)) {
		pos -= front * CAMERA_SPEED * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_J)) {
		pos -= right * CAMERA_SPEED * dt;
	}
	if (glfwGetKey(OpenGLSetup::window, GLFW_KEY_L)) {
		pos += right * CAMERA_SPEED * dt;
	}
	ship->setPosition(pos);
}
