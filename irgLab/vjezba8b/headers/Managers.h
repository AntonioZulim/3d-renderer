#pragma once

#include "Scene.h"
#include "Shader.h"

#include <GLFW/glfw3.h>

class InputManager {
public:
    static unsigned int width;
    static unsigned int height;
    static unsigned int numBezierSegments;
    static Transform* movable;
    static std::vector<Transform*> movable_candidates;
    static Curve* bezierCurve;

    static void framebuffer_size_callback(GLFWwindow* window, int Width, int Height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_button_callback();
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void poll_events(GLFWwindow* window);
    static void register_movable(Transform* transform);
    
private:
    static double xLastPos;
    static double yLastPos;
    static bool isFirstCursorCallback;
};

class ResourceManager {
public:
    Object* getScene(const std::string& name);
    Shader* getShader(const std::string& name);
    Texture* getTexture(const std::string& name);
};