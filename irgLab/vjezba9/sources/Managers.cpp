#include "Managers.h"
#include <iostream>

// ---------- InputManager ----------
unsigned int InputManager::width = 800;
unsigned int InputManager::height = 800;
unsigned int InputManager::numBezierSegments = 10;
Transform* InputManager::movable = nullptr;
std::vector<Transform*> InputManager::movable_candidates = std::vector<Transform*>();
Curve* InputManager::bezierCurve = nullptr;

double InputManager::xLastPos = 0;
double InputManager::yLastPos = 0;
bool InputManager::isFirstCursorCallback = true;

//funkcija koja se poziva prilikom mijenjanja velicine prozora, moramo ju povezati pomocu glfwSetFramebufferSizeCallback
void InputManager::framebuffer_size_callback(GLFWwindow* window, int Width, int Height)
{
	width = Width;
	height = Height;
	glViewport(0, 0, width, height);
}

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// inicijalno postavljanje
	if (isFirstCursorCallback) {
		xLastPos = xpos;
		yLastPos = ypos;
		isFirstCursorCallback = false;
		return;
	}

	double xDelta = xpos-xLastPos;
	double yDelta = ypos-yLastPos;

	movable->rotateFPS(xDelta, yDelta, true);
	xLastPos = xpos;
	yLastPos = ypos;
}

void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Camera* cam = dynamic_cast<Camera*>(movable);
	if (cam) {
		cam->setZoom(cam->getZoom() - yoffset*0.2);
	}
}

void InputManager::mouse_button_callback() {}

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		bezierCurve->addPoint(movable->getPosition());
		bezierCurve->updateCurve(numBezierSegments);
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		bezierCurve->clearPoints();
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		movable->registerAnimation(bezierCurve, 3.0f);
	}
	for (int i = 0; i<movable_candidates.size() && i<10; i++) {
		if (key == GLFW_KEY_0 + i && action == GLFW_PRESS) {
			InputManager::register_movable(movable_candidates[i]);
		}
	}
}

void InputManager::poll_events(GLFWwindow* window) {
	double sensitivity = 0.02;

	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS) {
		movable->localMove(glm::vec3(0, 0, -sensitivity));
	}
	if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS) {
		movable->localMove(glm::vec3(0, 0, sensitivity));
	}
	if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS) {
		movable->localMove(glm::vec3(-sensitivity, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS) {
		movable->localMove(glm::vec3(sensitivity, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_E)==GLFW_PRESS) {
		movable->localMove(glm::vec3(0, -sensitivity, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_Q)==GLFW_PRESS) {
		movable->localMove(glm::vec3(0, sensitivity, 0));
	}
}

void InputManager::register_movable(Transform* transform) {
	movable = transform;
}

// ---------- ResourceManager ----------
Object* ResourceManager::getScene(const std::string& name) { return nullptr; }
Shader* ResourceManager::getShader(const std::string& name) { return nullptr; }
Texture* ResourceManager::getTexture(const std::string& name) { return nullptr; }