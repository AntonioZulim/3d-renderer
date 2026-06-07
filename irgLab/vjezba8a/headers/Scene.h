#pragma once

#include "Geometry.h"

#define NOMINMAX
#include <glm/glm.hpp>

class Curve;

class Transform {
public:
    virtual ~Transform() = default;
    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;
    void rotateFPS(float xOffset, float yOffset, bool constrainPitch);
    void rotate(glm::mat4 rot);
    void globalMove(glm::vec3 delta);
    void localMove(glm::vec3 delta);
    void setOrientation(glm::vec3 n, glm::vec3 viewUp);
    void setPosition(glm::vec3 position);
    glm::vec3 getPosition();
    void scale(glm::vec3 scale);
    void update(float deltaTime);
    void registerAnimation(Curve* curve, float duration = 1.0f);

private:
    glm::vec3 mPosition = glm::vec3(0, 0, 0);
    glm::vec3 mFront = glm::vec3(0, 0, 1);
    glm::vec3 mUp = glm::vec3(0, 1, 0);
    glm::vec3 mRight = glm::vec3(1, 0, 0);
    glm::vec3 mScale = glm::vec3(1, 1, 1);
    float mPitch = 0;
    float mYaw = 0;
    Curve* animationCurve;
    float animationT;
    float animationDuration = 1.0f;
    bool animating = false;
};

class Light : public Transform{
public:
    glm::vec3 mIntensity;
    glm::vec3 mAmbientIntensity;
};

class Camera : public Transform{
public:
    virtual ~Camera() = default;
    glm::mat4 getPerspectiveMatrix(float width, float height) const;
    float getZoom();
    void setZoom(float value);
private:
    float mZoom = 1;
};

class Texture {
public:
    int getTextureID() const;

private:
    int mTextureID;
};

class Object : public Transform{
public:
    Object(Shader* shader);
    void addRenderable(Renderable* renderable);
    void clearRenderables();
    void render(glm::mat4 perspectiveMatrix, glm::mat4 viewMatrix, glm::vec3& eye, Light& light);

private:
    std::vector<Renderable*> mRenderables;
    Shader* mShader;
};

class Curve : public Object {
public:
    std::vector<glm::vec3> aproxCtrlPoints;
    std::vector<glm::vec3> interpCtrlPoints;
    using Object::Object;
    void addPoint(glm::vec3 point);
    void clearPoints();
    void updateCurve(unsigned int numBezierSegments, bool drawAproxCurve = true, bool drawInterpCurve = true, bool drawControlPolygon = true);
    static glm::vec3 calculateCurvePoint(const std::vector<glm::vec3>& controlPoints, float t);
    static std::vector<int> calculateBinCoefficients(int n);
    static std::vector<glm::vec3> Curve::calculateInterpControlPoints(std::vector<glm::vec3> verts);
};

class BezierBuilder {
public:
    //static Lines* makeInterpolationCurve(std::vector<glm::vec3> verts, unsigned int numSteps, glm::vec3 color = glm::vec3(0, 0, 0));
    static Lines* makeApproximationCurve(const std::vector<glm::vec3>& verts, unsigned int numSteps, glm::vec3 color = glm::vec3(0, 0, 0));
    static Lines* getControlPolygon(const std::vector<glm::vec3>& verts, glm::vec3 color = glm::vec3(0, 0, 0));
};

class Renderer {
public:
    Camera camera;
    Light light;

    Renderer();
    void render();
    void registerRenderable(Object* object);
    void update(float deltaTime);

private:
    std::vector<Object*> mObjects;
};