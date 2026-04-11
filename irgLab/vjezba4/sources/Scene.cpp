#include "Scene.h"

// Camera
glm::mat4 Camera::getPerspectiveMatrix(glm::vec2 screenSize) const { return glm::mat4(1.0f); }

// Renderer
void Renderer::render() {}
void Renderer::registerRenderable(Renderable* renderable) {}
void Renderer::update(float deltaTime) {}

// Texture
int Texture::getTextureID() const { return 0; }

// Transform
glm::mat4 Transform::getModelMatrix() const { return glm::mat4(1.0f); }
glm::mat4 Transform::getViewMatrix() const { return glm::mat4(1.0f); }
void Transform::rotateFPS(float xOffset, float yOffset, float constrainPitch) {}
void Transform::rotate(glm::mat4 rot) {}
void Transform::globalMove(glm::vec3 delta) {}
void Transform::localMove(glm::vec3 delta) {}
void Transform::setOrientation() {}
void Transform::setPosition(glm::vec3 position) {
	mPosition = position;
}
void Transform::scale(glm::vec3 scale) {
	mScale = scale;
}
void Transform::update(float deltaTime) {}
void Transform::registerAnimation(Lines* krivulja) {}

// Object
void Object::render(glm::mat4 perspectiveMatrix, glm::mat4 viewMatrix, Light* light) {}
