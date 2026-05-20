#include "MyGLM.h"

glm::mat4 MyGLM::translate3D(glm::vec3 translateVector) {
	return glm::mat4(1.0f);
}

glm::mat4 MyGLM::scale3D(glm::vec3 scaleVector) {
	return glm::mat4(1.0f);
}

glm::mat4 MyGLM::rotate3D(glm::vec3 axis, float angle) {
	return glm::mat4(1.0f);
}

glm::mat4 MyGLM::lookAtMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp) {
	return glm::mat4(1.0f);
}

glm::mat4 MyGLM::frustum(float l, float r, float b, float t, float n, float f) {
	return glm::mat4(1.0f);
}