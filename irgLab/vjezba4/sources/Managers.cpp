#include "Managers.h"

// InputManager
int InputManager::width = 800;
int InputManager::height = 800;
//funkcija koja se poziva prilikom mijenjanja velicine prozora, moramo ju povezati pomocu glfwSetFramebufferSizeCallback
void InputManager::framebuffer_size_callback(GLFWwindow* window, int Width, int Height)
{
	width = Width;
	height = Height;
	glViewport(0, 0, width, height);
}

void InputManager::mouse_callback() {}
void InputManager::scroll_callback() {}
void InputManager::mouse_button_callback() {}
void InputManager::poll_events() {}
void InputManager::register_movable(Transform* transform) {}

// ResourceManager
Object* ResourceManager::getScene(const std::string& name) { return nullptr; }
Shader* ResourceManager::getShader(const std::string& name) { return nullptr; }
Texture* ResourceManager::getTexture(const std::string& name) { return nullptr; }