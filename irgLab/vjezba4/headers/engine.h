#pragma once

#define NOMINMAX
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <utility>

#include <Shader.h>

class MyGLM {
public:
    glm::mat4 translate3D(glm::vec3 translateVector);
    glm::mat4 scale3D(glm::vec3 scaleVector);
    glm::mat4 rotate3D(glm::vec3 axis, float angle);
    glm::mat4 lookAtMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp);
    glm::mat4 frustum(float l, float r, float b, float t, float n, float f);
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

class Renderable {
public:
    virtual void draw() = 0;
    virtual ~Renderable() = default;

protected:
    unsigned int mVAO;
};

class Lines : public Renderable {
public:
    Lines(Lines* lines = nullptr);
    void draw() override;

private:
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mColors;
};

class BezierBuilder {
public:
    void makeInterpolationCurve();
    void makeApproximationPolygon();
    Lines* getControlPolygon();
};

class TriangleMesh : public Renderable {
public:
    TriangleMesh(const std::string& currPath, const std::string& resoucePath);
    void applyTransform(glm::mat4 mat);
    void getBoundingBox(std::pair<glm::vec3, glm::vec3>& bounds) const;
    void normalize();
    void draw() override;

private:
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mUvCoords;
    std::vector<int>       mIndices;
    unsigned int mVBO;
    unsigned int mEBO;
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

class Renderer {
public:
    void render();
    void registerRenderable(Renderable* renderable);
    void update(float deltaTime);

private:
    Camera mCamera;
    Light  mLight;
    std::vector<Renderable*> mRenderables;
};

class InputManager {
public:
    void framebuffer_size_callback();
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