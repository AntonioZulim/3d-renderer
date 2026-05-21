#include "Scene.h"
#include "Managers.h"

#include <glm/gtc/matrix_transform.hpp>

// ---------- Transform ----------
glm::mat4 Transform::getModelMatrix() const {
	glm::mat4 t = glm::translate(glm::mat4(1.0f), mPosition);
	glm::mat4 r = { 1.0f };
	r[0] = { mRight, 0 };
	r[1] = { mUp, 0 };
	r[2] = { mFront, 0 };
	glm::mat4 s = glm::scale(glm::mat4(1.0f), mScale);
	return t * r * s;
}

glm::mat4 Transform::getViewMatrix() const {
	//return glm::inverse(getModelMatrix()); // racunski zahtjevnije
	return glm::lookAt(mPosition, mPosition - mFront, mUp);
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
	glm::mat4 t = glm::translate(glm::mat4(1.0f), delta); // translacijska mat u lokalnom sustavu
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

glm::vec3 Transform::getPosition() {
	return mPosition;
}

void Transform::scale(glm::vec3 scale) {
	mScale = scale;
}

void Transform::update(float deltaTime) {
	if (animating) {
		animationT += deltaTime;
		if (animationT>=1.0f) {
			animationT = 1.0f;
			animating = false;
		}
		setPosition(Curve::calculateCurvePoint(animationCurve->interpCtrlPoints, animationT));
	}
}

void Transform::registerAnimation(Curve* curve) {
	animationCurve = curve;
	animationT = 0.0f;
	animating = true;
}

// ---------- Camera ----------
glm::mat4 Camera::getPerspectiveMatrix(float width, float height) const {
	float aspectRatio = width/height;
	float nearDist = 0.2f;
	return glm::frustum(-mZoom * aspectRatio * nearDist, mZoom * aspectRatio * nearDist, -mZoom * nearDist, mZoom * nearDist, nearDist, 100.0f);
}

float Camera::getZoom() {
	return mZoom;
}

void Camera::setZoom(float value) {
	if (value>=0.2 && value<=2)
		mZoom = value;
}

// ---------- Texture ----------
int Texture::getTextureID() const { return 0; }

// ---------- Object ----------
Object::Object(Shader* shader, Material* material, Texture* texture) {
	mShader = shader;
	mMaterial = material;
	mTexture = texture;
}

void Object::addRenderable(Renderable* renderable) {
	mRenderables.push_back(renderable);
}

void Object::clearRenderables() {
	for (Renderable* r : mRenderables) {
		delete(r);
	}
	mRenderables.clear();
}

void Object::render(glm::mat4 perspectiveMatrix, glm::mat4 viewMatrix, Light& light) {
	mShader->use();

	// matrice
	mShader->setUniform("modelMat", getModelMatrix(), false);
	mShader->setUniform("viewMat", viewMatrix, false);
	mShader->setUniform("perspectiveMat", perspectiveMatrix, false);

	if (mMaterial!=nullptr) {
		// svjetlo
		mShader->setUniform("light.position", light.getPosition());
		mShader->setUniform("light.intensity", light.mIntensity);
		mShader->setUniform("light.ambientIntensity", light.mAmbientIntensity);

		// materijal
		mShader->setUniform("material.ka", mMaterial->mAmbientColor);
		mShader->setUniform("material.kd", mMaterial->mDiffuseColor);
		mShader->setUniform("material.ks", mMaterial->mSpecularColor);
		mShader->setUniform("material.ksn", mMaterial->mSpecularExponent);
	}

	for (auto renderable : mRenderables) {
		renderable->draw();
	}
}

// ---------- Curve ----------
void Curve::addPoint(glm::vec3 point) {
	aproxCtrlPoints.push_back(point);
	if (aproxCtrlPoints.size()>=4) {
		interpCtrlPoints = Curve::calculateInterpControlPoints(std::vector<glm::vec3>(aproxCtrlPoints.end()-4, aproxCtrlPoints.end()));
	}
}

void Curve::clearPoints() {
	aproxCtrlPoints.clear();
	interpCtrlPoints.clear();
	clearRenderables();
}

void Curve::updateCurve(unsigned int numBezierSegments, bool drawAproxCurve, bool drawInterpCurve, bool drawControlPolygon) {
	clearRenderables();

	unsigned int n = aproxCtrlPoints.size()-1;
	if (drawAproxCurve) {
		Lines* aproxCurve = BezierBuilder::makeApproximationCurve(aproxCtrlPoints, numBezierSegments * n, glm::vec3(0, 0, 1));
		addRenderable(aproxCurve);
	}
	if (drawInterpCurve) {
		// moglo se pozvati makeInterpolationCurve s aproxCtrlPoints, ali ovako ne raèunamo kontrolne toèke svaki put
		Lines* interpCurve = BezierBuilder::makeApproximationCurve(interpCtrlPoints, numBezierSegments * n, glm::vec3(0, 1, 0));
		addRenderable(interpCurve);
	}
	if (drawControlPolygon) {
		Lines* controlPolygon = BezierBuilder::getControlPolygon(aproxCtrlPoints, glm::vec3(1, 0, 0));
		addRenderable(controlPolygon);
	}
}

std::vector<int> Curve::calculateBinCoefficients(int n) {
	std::vector<int> coefs = { 1 };
	int c = 1;
	for (int i = 1; i<=n; i++) {
		c = c * (n-i+1) / i;
		coefs.push_back(c);
	}
	return coefs;
}

std::vector<glm::vec3> Curve::calculateInterpControlPoints(std::vector<glm::vec3> verts) {
	if (verts.size()<4) {
		return {};
	}
	if (verts.size()>4) {
		verts.assign(verts.end()-4, verts.end());
	}
	glm::mat4x3 P = {
		verts[0],
		verts[1],
		verts[2],
		verts[3]
	};
	glm::mat4x4 B = {
		-1,  3, -3,  1,
		 3, -6,  3,  0,
		-3,  3,  0,  0,
		 1,  0,  0,  0
	};
	glm::mat4x4 T;
	for (int i = 0; i<=3; i++) {
		float t = i/3.0f;
		T[0][i] = t*t*t;
		T[1][i] = t*t;
		T[2][i] = t;
		T[3][i] = 1.0f;
	}

	glm::mat4x3 R = glm::transpose(glm::inverse(B) * glm::inverse(T) * glm::transpose(P));
	std::vector<glm::vec3> controlPoints = { R[0], R[1], R[2], R[3] };
	return controlPoints;
}

glm::vec3 Curve::calculateCurvePoint(const std::vector<glm::vec3>& controlPoints, float t)  {
	int n = controlPoints.size()-1;
	std::vector<int> coefs = calculateBinCoefficients(n);

	glm::vec3 p = glm::vec3(0, 0, 0);
	for (int i = 0; i<=n; i++) {
		float factor = coefs[i] * std::pow(t, i) * std::pow(1-t, n-i);
		p.x += controlPoints[i].x * factor;
		p.y += controlPoints[i].y * factor;
		p.z += controlPoints[i].z * factor;
	}
	return p;
}

// ---------- BezierBuilder ----------
//Lines* BezierBuilder::makeInterpolationCurve(std::vector<glm::vec3> verts, unsigned int numSteps, glm::vec3 color) {
//	if (verts.size()<4) {
//		return new Lines();
//	}
//	return makeApproximationCurve(Curve::calculateInterpControlPoints(verts), numSteps, color);
//}

Lines* BezierBuilder::makeApproximationCurve(const std::vector<glm::vec3>& verts, unsigned int numSteps, glm::vec3 color) {
	std::vector<glm::vec3> points;

	for (int step = 0; step<=numSteps; step++) {
		float t = 1.0/numSteps * step;
		points.push_back(Curve::calculateCurvePoint(verts, t));
	}
	std::vector<glm::vec3> colors;
	for (int i = 0; i<=numSteps; i++) {
		colors.push_back(color);
	}
	return new Lines(points, colors);
}

Lines* BezierBuilder::getControlPolygon(const std::vector<glm::vec3>& verts, glm::vec3 color) {
	std::vector<glm::vec3> colors;
	for (int i = 0; i<verts.size(); i++) {
		colors.push_back(color);
	}
	return new Lines(verts, colors);
}

// ---------- Renderer ----------
Renderer::Renderer() {
	InputManager::register_movable(&camera);
	InputManager::movable_candidates.push_back(&camera);
}

void Renderer::render() {
	for (Object* obj : mObjects) {
		obj->render(camera.getPerspectiveMatrix(InputManager::width, InputManager::height), camera.getViewMatrix(), light);
	}
}

void Renderer::registerRenderable(Object* object) {
	mObjects.push_back(object);
}

void Renderer::update(float deltaTime) {}
