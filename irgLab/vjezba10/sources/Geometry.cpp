#include "Geometry.h"

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <filesystem>

// ---------- Texture ----------
Texture::Texture(std::string path) {
	mTextureID = 0;
	mPath = path;

	int width, height, nrChannels;
	unsigned char* data;
	data = stbi_load(mPath.c_str(), &width, &height, &nrChannels, 0);

	if (!data) {
		std::cout << "Failed to load texture: " << mPath << std::endl;
		std::cout << "Error message: " << stbi_failure_reason() << std::endl;
		return;
	}
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	switch (nrChannels) {
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	default:
		std::cout << "Unrecognised texture format: " << mPath << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}

int Texture::getTextureID() const {
	return mTextureID;
}

Texture::~Texture() {
	if (mTextureID!=0) {
		glDeleteTextures(1, &mTextureID);
	}
}

// ---------- Material ----------
Material::~Material() {
	delete mDiffuseMap;
}

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

void Lines::draw(Shader* shader) {
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
		aiProcess_GenSmoothNormals
	);

	if (!scene) {
		std::cerr << importer.GetErrorString();
	}
	else if (scene->HasMeshes()) {
		for (int x = 0; x<scene->mNumMeshes; x++) {
			aiMesh* mesh = scene->mMeshes[x];
			unsigned int offset = mVertices.size();

			// spremanje vrhova i normala
			for (int i = 0; i < mesh->mNumVertices; i++) {
				mVertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
				mNormals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
			}

			// spremanje indeksa
			for (int i = 0; i < mesh->mNumFaces; i++) {
				for (int j = 0; j<mesh->mFaces[i].mNumIndices; j++) {
					mIndices.push_back(mesh->mFaces[i].mIndices[j]+offset);
				}
			}

			// spremanje UV mape i tangenti
			if (mesh->HasTextureCoords(0)) {
				for (int i = 0; i < mesh->mNumVertices; i++) {
					mUvCoords.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
					mTangents.push_back(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
					mBitangents.push_back(glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z));
				}
			}
		}

		// spremanje materijala
		if (scene->HasMaterials()) {
			aiMaterial* mat = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex];

			aiString texturePosition;
			if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texturePosition)) {
				std::string texturePath = texturePosition.C_Str();
				std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
				mMaterial.mDiffuseMap = new Texture((objPath.parent_path() / std::filesystem::path(texturePath).lexically_normal()).string());
				mMaterial.mFlags |= 1 << 0;
			}
			if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), texturePosition)) {
				std::string texturePath = texturePosition.C_Str();
				std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
				mMaterial.mAOMap = new Texture((objPath.parent_path() / std::filesystem::path(texturePath).lexically_normal()).string());
				mMaterial.mFlags |= 1 << 1;
			}
			if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), texturePosition)) {
				std::string texturePath = texturePosition.C_Str();
				std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
				mMaterial.mSpecularMap = new Texture((objPath.parent_path() / std::filesystem::path(texturePath).lexically_normal()).string());
				mMaterial.mFlags |= 1 << 2;
			}
			if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_SHININESS, 0), texturePosition)) {
				std::string texturePath = texturePosition.C_Str();
				std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
				mMaterial.mGlossinessMap = new Texture((objPath.parent_path() / std::filesystem::path(texturePath).lexically_normal()).string());
				mMaterial.mFlags |= 1 << 3;
			}
			if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), texturePosition)) {
				std::string texturePath = texturePosition.C_Str();
				std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
				mMaterial.mNormalMap = new Texture((objPath.parent_path() / std::filesystem::path(texturePath).lexically_normal()).string());
				mMaterial.mFlags |= 1 << 4;
			}
			if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), texturePosition)) {
				std::string texturePath = texturePosition.C_Str();
				std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
				mMaterial.mHeightMap = new Texture((objPath.parent_path() / std::filesystem::path(texturePath).lexically_normal()).string());
				mMaterial.mFlags |= 1 << 5;
			}

			aiColor3D ambientK, diffuseK, specularK;
			float shininessK;

			mat->Get(AI_MATKEY_COLOR_AMBIENT, ambientK);
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseK);
			mat->Get(AI_MATKEY_COLOR_SPECULAR, specularK);
			mat->Get(AI_MATKEY_SHININESS, shininessK);
			mMaterial.mAmbientColor = glm::vec3(ambientK.r, ambientK.g, ambientK.b);
			if (mMaterial.mAmbientColor == glm::vec3(1, 1, 1)) {
				mMaterial.mAmbientColor = glm::vec3(0.2);
			}
			mMaterial.mDiffuseColor = glm::vec3(diffuseK.r, diffuseK.g, diffuseK.b);
			mMaterial.mSpecularColor = glm::vec3(specularK.r, specularK.g, specularK.b);
			mMaterial.mSpecularExponent = shininessK;
		}

		// generiranje VAO, VBO, EBO
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mPosVBO);
		glGenBuffers(1, &mNormVBO);
		glGenBuffers(1, &mUvVBO);
		glGenBuffers(1, &mTangVBO);
		glGenBuffers(1, &mBitangVBO);
		glGenBuffers(1, &mEBO);

		// inicijalizacija VBO i EBO
		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mPosVBO);
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), mVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, mNormVBO);
		glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(glm::vec3), mNormals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		if (!mUvCoords.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, mUvVBO);
			glBufferData(GL_ARRAY_BUFFER, mUvCoords.size() * sizeof(glm::vec2), mUvCoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, mTangVBO);
			glBufferData(GL_ARRAY_BUFFER, mTangents.size() * sizeof(glm::vec3), mTangents.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(3);

			glBindBuffer(GL_ARRAY_BUFFER, mBitangVBO);
			glBufferData(GL_ARRAY_BUFFER, mBitangents.size() * sizeof(glm::vec3), mBitangents.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(4);
		}

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
	glBindBuffer(GL_ARRAY_BUFFER, mPosVBO);
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

void TriangleMesh::draw(Shader* shader) {
	shader->setUniform("flags", mMaterial.mFlags);

	// materijal
	shader->setUniform("material.ka", mMaterial.mAmbientColor);
	shader->setUniform("material.kd", mMaterial.mDiffuseColor);
	shader->setUniform("material.ks", mMaterial.mSpecularColor);
	shader->setUniform("material.ksn", mMaterial.mSpecularExponent);

	glBindVertexArray(mVAO);
	if (mMaterial.mDiffuseMap && mMaterial.mDiffuseMap->getTextureID()!=0) {
		shader->setUniform("diffuseMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mMaterial.mDiffuseMap->getTextureID());
	}
	if (mMaterial.mAOMap && mMaterial.mAOMap->getTextureID()!=0) {
		shader->setUniform("AOMap", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mMaterial.mAOMap->getTextureID());
	}
	if (mMaterial.mSpecularMap && mMaterial.mSpecularMap->getTextureID()!=0) {
		shader->setUniform("specularMap", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mMaterial.mSpecularMap->getTextureID());
	}
	if (mMaterial.mGlossinessMap && mMaterial.mGlossinessMap->getTextureID()!=0) {
		shader->setUniform("glossinessMap", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mMaterial.mGlossinessMap->getTextureID());
	}
	if (mMaterial.mNormalMap && mMaterial.mNormalMap->getTextureID()!=0) {
		shader->setUniform("normalMap", 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mMaterial.mNormalMap->getTextureID());
	}
	if (mMaterial.mHeightMap && mMaterial.mHeightMap->getTextureID()!=0) {
		shader->setUniform("heightMap", 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, mMaterial.mHeightMap->getTextureID());
	}
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, (void *) 0);
	glBindVertexArray(0);
}

Material* TriangleMesh::getMaterial() {
	return &mMaterial;
}