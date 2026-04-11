// Local Headers
#include "Shader.h"
#include "FPSManager.h"
#include "Geometry.h"
#include "Managers.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>

Shader* loadShader(char* path, char* naziv) {
	std::filesystem::path sPath = std::filesystem::path(path).parent_path() / "shaders" / naziv;
	std::string pathVert(sPath.string() + ".vert");
	std::string pathFrag(sPath.string() + ".frag");

	return new Shader(pathVert.c_str(), pathFrag.c_str());
}

int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	/*********************************************************************************************/
	//postavljanje OpenGL konteksta, dohvacanje dostupnih OpenGL naredbi
	GLFWwindow* window;

	glfwInit();
	gladLoadGL();

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	window = glfwCreateWindow(InputManager::width, InputManager::height, "Zadatak 4", nullptr, nullptr);
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe iscrtavanje

	glfwSwapInterval(0); //ne cekaj nakon iscrtavanja (vsync)
	FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak 4");

	glfwSetFramebufferSizeCallback(window, InputManager::framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora

	Shader* shader = loadShader(argv[0], "shader");

	// dodavanje mesha
	TriangleMesh* mesh = new TriangleMesh(argv[0], "glava\\glava.obj");
	mesh->normalize();

	/*********************************************************************************************/
	//glavna petlja za prikaz
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		//pobrisi platno
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, InputManager::width, InputManager::height);

		// iscrtavanje mesha
		shader->use();
		mesh->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}   
		
	delete shader;

	glfwTerminate();

    return EXIT_SUCCESS;
}
