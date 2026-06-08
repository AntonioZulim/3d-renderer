#pragma once

#include "Shader.h"

#define NOMINMAX
#include <glm/glm.hpp>

class Texture {
public:
    Texture(std::string path);
    int getTextureID() const;
    ~Texture();

private:
    unsigned int mTextureID;
    std::string mPath;
};

class Material {
public:
    Texture* mDiffuseMap = nullptr;
    Texture* mAOMap = nullptr;
    Texture* mSpecularMap = nullptr;
    Texture* mGlossinessMap = nullptr;
    Texture* mNormalMap = nullptr;
    Texture* mHeightMap = nullptr;
    glm::vec3 mAmbientColor = glm::vec3(0,0,0);
    glm::vec3 mDiffuseColor = glm::vec3(0, 0, 0);
    glm::vec3 mSpecularColor = glm::vec3(0, 0, 0);
    float mSpecularExponent = 0;
    unsigned int mFlags = 0;
    ~Material();
};

class Renderable {
public:
    virtual void draw(Shader* shader) = 0;
    virtual ~Renderable() = default;

protected:
    unsigned int mVAO;
};

class Lines : public Renderable {
public:
    Lines(std::vector<glm::vec3> verts = {}, std::vector<glm::vec3> colors = {});
    void draw(Shader* shader) override;
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
    void draw(Shader* shader) override;
    Material* getMaterial();

private:
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec3> mTangents;
    std::vector<glm::vec3> mBitangents;
    std::vector<glm::vec2> mUvCoords;
    std::vector<int>       mIndices;
    unsigned int mPosVBO;
    unsigned int mNormVBO;
    unsigned int mUvVBO;
    unsigned int mTangVBO;
    unsigned int mBitangVBO;
    unsigned int mEBO;
    Material mMaterial;
};