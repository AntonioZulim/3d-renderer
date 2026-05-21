#include "Geometry.h"

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>

#include <iostream>
#include <filesystem>

// ---------- Lines ----------
Lines::Lines(std::vector<glm::vec3> verts, std::vector<glm::vec3> colors)
	: mVertices(std::move(verts)), mColors(std::move(colors)) {
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mPosVBO);
	glGenBuffers(1, &mColVBO);

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mPosVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, mColVBO);
	glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Lines::draw() {
	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mPosVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), mVertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mColVBO);
	glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(glm::vec3), mColors.data(), GL_DYNAMIC_DRAW);

	glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());

	glBindVertexArray(0);
}

// ---------- TriangleMesh ----------
TriangleMesh::TriangleMesh(const std::string& currPath, const std::string& resourcePath) {
	Assimp::Importer importer;
	std::filesystem::path objPath = std::filesystem::path(currPath).parent_path() / "resources" / resourcePath;
	const aiScene* scene = importer.ReadFile(objPath.string().c_str(),
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
		aiMesh* mesh = scene->mMeshes[0];

		// spremanje vrhova i normala
		for (int i = 0; i < mesh->mNumVertices; i++) {
			mVertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
			mNormals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		}
		for (int i = 0; i < mesh->mNumFaces; i++) {
			for (int j = 0; j<mesh->mFaces[i].mNumIndices; j++) {
				mIndices.push_back(mesh->mFaces[i].mIndices[j]);
			}
		}

		// spremanje materijala
		if (scene->HasMaterials()) {
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

			glm::vec3 ambientColor;
			aiColor3D ambientK, diffuseK, specularK;
			float shininessK;

			mat->Get(AI_MATKEY_COLOR_AMBIENT, ambientK);
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseK);
			mat->Get(AI_MATKEY_COLOR_SPECULAR, specularK);
			mat->Get(AI_MATKEY_SHININESS, shininessK);
			mMaterial.mAmbientColor = glm::vec3(ambientK.r, ambientK.g, ambientK.b);
			mMaterial.mDiffuseColor = glm::vec3(diffuseK.r, diffuseK.g, diffuseK.b);
			mMaterial.mSpecularColor = glm::vec3(specularK.r, specularK.g, specularK.b);
			mMaterial.mSpecularExponent = shininessK;
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

	// osvjezi podatke u gpu-u
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), mVertices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void TriangleMesh::getBoundingBox(std::pair<glm::vec3, glm::vec3>& bounds) const {
	bounds.first = mVertices[0]; // minimum
	bounds.second = mVertices[0]; // maksimum
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

Material* TriangleMesh::getMaterial() {
	return &mMaterial;
}