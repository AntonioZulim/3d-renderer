#include "engine.h"

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>

#include <iostream>

// MyGLM
glm::mat4 MyGLM::translate3D(glm::vec3 translateVector) { return glm::mat4(1.0f); }
glm::mat4 MyGLM::scale3D(glm::vec3 scaleVector) { return glm::mat4(1.0f); }
glm::mat4 MyGLM::rotate3D(glm::vec3 axis, float angle) { return glm::mat4(1.0f); }
glm::mat4 MyGLM::lookAtMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp) { return glm::mat4(1.0f); }
glm::mat4 MyGLM::frustum(float l, float r, float b, float t, float n, float f) { return glm::mat4(1.0f); }

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

// Texture
int Texture::getTextureID() const { return 0; }

// Camera
glm::mat4 Camera::getPerspectiveMatrix(glm::vec2 screenSize) const { return glm::mat4(1.0f); }

// Lines
Lines::Lines(Lines* lines) {}
void Lines::draw() {}

// BezierBuilder
void BezierBuilder::makeInterpolationCurve() {}
void BezierBuilder::makeApproximationPolygon() {}
Lines* BezierBuilder::getControlPolygon() { return nullptr; }

// TriangleMesh
TriangleMesh::TriangleMesh(const std::string& currPath, const std::string& resourcePath) {
	Assimp::Importer importer;

	std::string path(currPath);
	std::string dirPath(path, 0, path.find_last_of("\\/"));
	std::string resPath(dirPath);
	resPath.append("\\resources"); //za linux pretvoriti u forwardslash
	std::string objPath(resPath);
	objPath.append("\\" + resourcePath); //za linux pretvoriti u forwardslash

	const aiScene* scene = importer.ReadFile(objPath.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FlipUVs |
		aiProcess_GenNormals

	);

	if (!scene) {
		std::cerr << importer.GetErrorString();
	}
	else if (scene->HasMeshes()) {
		// spremanje podataka
		aiMesh* mesh = scene->mMeshes[0];
		for (int i = 0; i < mesh->mNumVertices; i++) {
			mVertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		}
		for (int i = 0; i < mesh->mNumFaces; i++) {
			for (int j = 0; j<mesh->mFaces[i].mNumIndices; j++) {
				mIndices.push_back(mesh->mFaces[i].mIndices[j]);
			}
		}

		// generiranje VAO, VBO, EBO
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glGenBuffers(1, &mEBO);

		// inicijalizacija VBO i EBO
		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), mVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
}

void TriangleMesh::applyTransform(glm::mat4 mat) {
	for (int i = 0; i<mVertices.size(); i++) {
		mVertices[i] = mat * glm::vec4(mVertices[i], 1);
	}

	// update data in gpu
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), mVertices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void TriangleMesh::getBoundingBox(std::pair<glm::vec3, glm::vec3>& bounds) const {
	bounds.first = mVertices[0];
	bounds.second = mVertices[0];
	for (int i = 1; i<mVertices.size(); i++) {
		bounds.first = glm::min(bounds.first, mVertices[i]);
		bounds.second = glm::max(bounds.second, mVertices[i]);
	}
}

void TriangleMesh::normalize() {
	// trazenje centra
	std::pair<glm::vec3, glm::vec3> bounds;
	getBoundingBox(bounds);
	glm::vec3 center = (bounds.first + bounds.second)/2.0f;

	// trazenje maksimalnog raspona
	glm::vec3 diff = bounds.second-bounds.first;
	float maxRange = std::max({ diff.x, diff.y, diff.z });

	// translacijska matrica T
	glm::mat4 T = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-center.x, -center.y, -center.z, 1
	};

	// skalirajuca matrica S
	glm::mat4 S = glm::mat4(1);
	S *= 2.0f/maxRange;

	applyTransform(S*T);
}

void TriangleMesh::draw() {
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, (void *) 0);
	glBindVertexArray(0);
}

// Object
void Object::render(glm::mat4 perspectiveMatrix, glm::mat4 viewMatrix, Light* light) {}

// Renderer
void Renderer::render() {}
void Renderer::registerRenderable(Renderable* renderable) {}
void Renderer::update(float deltaTime) {}

// InputManager
void InputManager::framebuffer_size_callback() {}
void InputManager::mouse_callback() {}
void InputManager::scroll_callback() {}
void InputManager::mouse_button_callback() {}
void InputManager::poll_events() {}
void InputManager::register_movable(Transform* transform) {}

// ResourceManager
Object* ResourceManager::getScene(const std::string& name) { return nullptr; }
Shader* ResourceManager::getShader(const std::string& name) { return nullptr; }
Texture* ResourceManager::getTexture(const std::string& name) { return nullptr; }