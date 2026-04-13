#pragma once

#include "Shader.h"

#define NOMINMAX
#include <glm/glm.hpp>

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