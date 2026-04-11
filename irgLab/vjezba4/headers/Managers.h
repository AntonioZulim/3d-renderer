#pragma once

#include "Scene.h"
#include "Shader.h"

#include <GLFW/glfw3.h>

class InputManager {
public:
    static int width;
    static int height;
    static void framebuffer_size_callback(GLFWwindow* window, int Width, int Height);

    void mouse_callback();
    void scroll_callback();
    void mouse_button_callback();
    void poll_events();
    void register_movable(Transform* transform);
};

class ResourceManager {
public:
    Object* getScene(const std::string& name);
    Shader* getShader(const std::string& name);
    Texture* getTexture(const std::string& name);
};