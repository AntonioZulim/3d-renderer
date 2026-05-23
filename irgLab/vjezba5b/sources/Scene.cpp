#include "Scene.h"
#include "Managers.h"
#include "MyGLM.h"

#include <glm/gtc/matrix_transform.hpp>

// ---------- Transform ----------
glm::mat4 Transform::getModelMatrix() const {
	glm::mat4 t = MyGLM::translate3D(mPosition);
	glm::mat4 r = { 1.0f };
	r[0] = { mRight, 0 };
	r[1] = { mUp, 0 };
	r[2] = { mFront, 0 };
	glm::mat4 s = MyGLM::scale3D(mScale);
	return t * r * s;
}

glm::mat4 Transform::getViewMatrix() const {
	//return glm::inverse(getModelMatrix()); // racunski zahtjevnije
	return MyGLM::lookAtMatrix(mPosition, mPosition - mFront, mUp);
}

void Transform::rotateFPS(float xOffset, float yOffset, bool constrainPitch) {
	float sensitivity = 0.005f;

	mPitch += yOffset*sensitivity;
	mYaw -= xOffset*sensitivity;
	mPitch = glm::clamp(mPitch, glm::radians(-89.0f), glm::radians(89.0f));
	glm::vec3 newFront;
	newFront.x = cos(mPitch) * sin(mYaw);
	newFront.y = sin(mPitch);
	newFront.z = cos(mPitch) * cos(mYaw);
	setOrientation(newFront, glm::vec3(0, 1, 0));
}

void Transform::rotate(glm::mat4 rot) {
	mRight = glm::normalize(rot * glm::vec4(mRight, 0));
	mUp = glm::normalize(rot * glm::vec4(mUp, 0));
	mFront = glm::normalize(rot * glm::vec4(mFront, 0));
}

void Transform::globalMove(glm::vec3 delta) {
	mPosition += delta;
}

void Transform::localMove(glm::vec3 delta) {
	glm::mat4 t = MyGLM::translate3D(delta); // translacijska mat u lokalnom sustavu
	glm::mat4 r = { 1.0f }; // rotiranje iz lokalnog u globalni sustav
	r[0] = { mRight, 0 };
	r[1] = { mUp, 0 };
	r[2] = { mFront, 0 };
	glm::mat4 globalT = r * t; // translacijska mat u globalnom sustavu
	glm::vec3 globalDelta = globalT[3];
	globalMove(globalDelta);
}

void Transform::setOrientation(glm::vec3 n, glm::vec3 viewUp) {
	mFront = glm::normalize(n);
	mRight = glm::normalize(glm::cross(viewUp, mFront));
	mUp = glm::normalize(glm::cross(mFront, mRight));
}

void Transform::setPosition(glm::vec3 position) {
	mPosition = position;
}

void Transform::scale(glm::vec3 scale) {
	mScale = scale;
}

void Transform::update(float deltaTime) {}

void Transform::registerAnimation(Lines* krivulja) {}

// ---------- Camera ----------
glm::mat4 Camera::getPerspectiveMatrix(float width, float height) const {
	float aspectRatio = width/height;
	float nearDist = 0.2f;
	return MyGLM::frustum(-mZoom * aspectRatio * nearDist, mZoom * aspectRatio * nearDist, -mZoom * nearDist, mZoom * nearDist, nearDist, 100.0f);
}

float Camera::getZoom() {
	return mZoom;
}

void Camera::setZoom(float value) {
	if (value>=0.2 && value <=2)
		mZoom = value;
}

// ---------- Texture ----------
int Texture::getTextureID() const { return 0; }

// ---------- Object ----------
Object::Object(Shader* shader) {
	mShader = shader;
	mMaterial = nullptr;
	mTexture = nullptr;
}

Object::Object(Shader* shader, Material* material, Texture* texture) {
	mShader = shader;
	mMaterial = material;
	mTexture = texture;
}

void Object::addRenderable(Renderable* renderable) {
	mRenderables.push_back(renderable);
}

void Object::render(glm::mat4 perspectiveMatrix, glm::mat4 viewMatrix, Light* light) {
	mShader->use();
	mShader->setUniform("modelMat", getModelMatrix(), false);
	mShader->setUniform("viewMat", viewMatrix, false);
	mShader->setUniform("perspectiveMat", perspectiveMatrix, false);
	for (auto renderable : mRenderables) {
		renderable->draw();
	}
}

// ---------- Renderer ----------
Renderer::Renderer() {
	InputManager::register_movable(&camera);
	InputManager::movable_candidates.push_back(&camera);
}

void Renderer::render() {
	for (Object* obj : mObjects) {
		obj->render(camera.getPerspectiveMatrix(InputManager::width, InputManager::height), camera.getViewMatrix(), nullptr);
	}
}

void Renderer::registerRenderable(Object* object) {
	mObjects.push_back(object);
}

void Renderer::update(float deltaTime) {}
