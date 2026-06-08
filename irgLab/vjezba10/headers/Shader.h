#pragma once

#define NOMINMAX
#include <glm/glm.hpp>
#include <glad/glad.h>

#include <string>

#include <fstream>
#include <sstream>

class Shader
{
private:
	void checkCompilerErrors(unsigned int shader, std::string type);

public:

	unsigned int ID;

	Shader(const char* path, const char* vert_name, const char* frag_name = nullptr, const char* geom_name = nullptr);

	~Shader();

	void use();
	void setUniform(const std::string &name, bool value)const;
	void setUniform(const std::string &name, int value)const;
	void setUniform(const std::string &name, unsigned int value)const;
	void setUniform(const std::string &name, float value)const;
	void setUniform(const std::string &name, glm::vec3 value)const;
	void setUniform(const std::string &name, glm::mat4 mat, bool transpose)const;
};

