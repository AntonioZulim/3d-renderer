#pragma once

#include "Geometry.h"

#define NOMINMAX
#include <glm/glm.hpp>

class Light {
private:
    glm::vec3 mIntensity;
    glm::vec3 mAmbientIntensity;
};

class Camera {
public:
    glm::mat4 getPerspectiveMatrix(glm::vec2 screenSize) const;

private:
    float mZoom;
};

class Renderer {
public:
    void render();
    void registerRenderable(Renderable* renderable);
    void update(float deltaTime);

private:
    Camera camera;
    Light  mLight;
    std::vector<Renderable*> mRenderables;
};

class Texture {
public:
    int getTextureID() const;

private:
    int mTextureID;
};

class Material {
public:
    Shader* shader = nullptr;
    Texture* texture = nullptr;

private:
    glm::vec3 mAmbientColor;
    glm::vec3 mDiffuseColor;
    glm::vec3 mSpecularColor;
    float     mSpecularExponent;
};

class Transform {
public:
    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;
    void rotateFPS(float xOffset, float yOffset, float constrainPitch);
    void rotate(glm::mat4 rot);
    void globalMove(glm::vec3 delta);
    void localMove(glm::vec3 delta);
    void setOrientation();
    void setPosition(glm::vec3 position);
    void scale(glm::vec3 scale);
    void update(float deltaTime);
    void registerAnimation(Lines* krivulja);

private:
    glm::vec3 mPosition;
    glm::vec3 mFront;
    glm::vec3 mUp;
    glm::vec3 mRight;
    glm::vec3 mScale;
};

class Object {
public:
    void render(glm::mat4 perspectiveMatrix, glm::mat4 viewMatrix, Light* light);

private:
    Material  mMaterial;
    Transform mTransform;
    std::vector<Renderable*> mRenderables;
};