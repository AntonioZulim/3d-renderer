// Local Headers
#include "Shader.h"
#include "FPSManager.h"
#include "engine.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>

int width = 800, height = 800;
glm::vec3 drawingColor = glm::vec3(1, 1, 1);
glm::vec3 adjustingColor = glm::vec3(1.0, 0, 0);
float mouseX = 0, mouseY = 0;

std::vector<float> polygonVertices;
std::vector<unsigned int> polygonIndices;
std::vector<unsigned int> linesIndices;
GLuint VAO[3];
GLuint VBO[2];
GLuint EBO[2];

//malo je nespretno napravljeno jer ne koristimo c++17, a treba podrzati i windows i linux,
//slobodno pozivajte new Shader(...); direktno
Shader* loadShader(char* path, char* naziv) {
	std::string sPath(path);
	std::string pathVert;
	std::string pathFrag;

	pathVert.append(path, sPath.find_last_of("\\/") + 1);
	pathFrag.append(path, sPath.find_last_of("\\/") + 1);
	if (pathFrag[pathFrag.size() - 1] == '/') {
		pathVert.append("shaders/");
		pathFrag.append("shaders/");
	}
	else if (pathFrag[pathFrag.size() - 1] == '\\') {
		pathVert.append("shaders\\");
		pathFrag.append("shaders\\");
	}
	else {
		std::cerr << "nepoznat format pozicije shadera";
		exit(1);
	}

	pathVert.append(naziv);
	pathVert.append(".vert");
	pathFrag.append(naziv);
	pathFrag.append(".frag");

	return new Shader(pathVert.c_str(), pathFrag.c_str());
}

//funkcija koja se poziva prilikom mijenjanja velicine prozora, moramo ju povezati pomocu glfwSetFramebufferSizeCallback
void framebuffer_size_callback(GLFWwindow * window, int Width, int Height)
{
	width = Width;
	height = Height;
	glViewport(0, 0, width, height);
}

int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	/*********************************************************************************************/
	//postavljanje OpenGL konteksta, dohvacanje dostupnih OpenGL naredbi
	GLFWwindow* window;

	glfwInit();
	gladLoadGL();

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);


	window = glfwCreateWindow(width, height, "Zadatak 4", nullptr, nullptr);
	// provjeri je li se uspio napraviti prozor
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// dohvati sve dostupne OpenGL funkcije
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD");
		exit(-1);
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));


	//glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE); //ukljuci uklanjanje straznjih poligona -- za ovaj primjer je iskljuceno
	//glCullFace(GL_BACK); 

	glClearColor(0.15f, 0.1f, 0.1f, 1.0f); //boja brisanja platna izmedu iscrtavanja dva okvira


	glfwSwapInterval(0); //ne cekaj nakon iscrtavanja (vsync)

	FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak 4");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe iscrtavanje
	Shader* shader = loadShader(argv[0], "shader"); // 

	// dodavanje mesha
	TriangleMesh* mesh = new TriangleMesh(argv[0], "glava\\glava.obj");
	mesh->normalize();

	/*********************************************************************************************/
	//glavna petlja za prikaz
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		//pobrisi platno
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);

		// iscrtavanje mesha
		glUseProgram(shader->ID);
		mesh->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}   
		
	delete shader;
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(2, EBO);
	glDeleteVertexArrays(3, VAO);

	glfwTerminate();

    return EXIT_SUCCESS;
}
