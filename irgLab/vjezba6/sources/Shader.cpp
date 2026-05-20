#include "Shader.h"

#include <iostream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

void Shader::checkCompilerErrors(unsigned int shader, std::string type)
{
	int success;
	char infolog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(shader, 1024, nullptr, infolog);
			fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n-----------------------------------------------------\n", type.c_str(), infolog);
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, nullptr, infolog);
			fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n-------------------------------------------------------\n", type.c_str(), infolog);
		}
	}
}

Shader::Shader(const char* path,const char* vertName, const char* fragName, const char* geomName)
{
	std::filesystem::path sPath = std::filesystem::path(path).parent_path() / "shaders";
	std::string pathVert = (sPath / vertName).string() + ".vert";
	std::string pathFrag = (sPath / fragName).string() + ".frag";
	std::string pathGeom;
	if (geomName)
		pathGeom = (sPath / geomName).string() + ".geom";
	
	const char* vertexPath = pathVert.c_str();
	const char* fragmentPath = pathFrag.c_str();
	const char* geometryPath = pathGeom.c_str();

	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// vertex
		vShaderFile.open(vertexPath); // open file
		std::stringstream vShaderStream;
		vShaderStream << vShaderFile.rdbuf(); // read file's buffer content into stream
		vShaderFile.close(); // close file handler
		vertexCode = vShaderStream.str(); // convert stream into string

		// fragment
		fShaderFile.open(fragmentPath);
		std::stringstream fShaderStream;
		fShaderStream << fShaderFile.rdbuf();
		fShaderFile.close();
		fragmentCode = fShaderStream.str();

		// geometry
		if (geomName) {
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
		
	}
	catch (std::ifstream::failure e)
	{
		fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment, geometry;
	int success;
	char infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompilerErrors(vertex, "VERTEX");

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompilerErrors(fragment, "FRAGMENT");

	// geometry Shader
	if (geomName) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompilerErrors(geometry, "GEOMETRY");
	}

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geomName)
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompilerErrors(ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setUniform(const std::string & name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setUniform(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setUniform(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setUniform(const std::string & name, glm::vec3 value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string & name, glm::mat4 mat, bool transpose) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, transpose, glm::value_ptr(mat));
}
