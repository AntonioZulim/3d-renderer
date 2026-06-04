#pragma once

#include "Shader.h"

#define NOMINMAX
#include <glm/glm.hpp>

class Material {
public:
    glm::vec3 mAmbientColor;
    glm::vec3 mDiffuseColor;
    glm::vec3 mSpecularColor;
    float     mSpecularExponent;
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
    Lines(std::vector<glm::vec3> verts = {}, std::vector<glm::vec3> colors = {});
    void draw() override;
private:
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mColors;
    unsigned int mPosVBO;
    unsigned int mColVBO;
};

class TriangleMesh : public Renderable {
public:
    TriangleMesh(const std::string& currPath, const std::string& resoucePath);
    void applyTransform(glm::mat4 mat);
    void getBoundingBox(std::pair<glm::vec3, glm::vec3>& bounds) const;
    void normalize();
    void draw() override;
    Material* getMaterial();

private:
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mUvCoords;
    std::vector<int>       mIndices;
    unsigned int mPosVBO;
    unsigned int mNormVBO;
    unsigned int mEBO;
    Material mMaterial;
};