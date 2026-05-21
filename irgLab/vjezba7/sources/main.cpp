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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // zakljucaj i sakrij kursor
	glfwSwapInterval(0); //ne cekaj nakon iscrtavanja (vsync)
	FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak 6");

	// callback funkcije
	glfwSetScrollCallback(window, InputManager::scroll_callback);
	glfwSetCursorPosCallback(window, InputManager::mouse_callback);
	glfwSetKeyCallback(window, InputManager::key_callback);
	glfwSetFramebufferSizeCallback(window, InputManager::framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora

	// stvaranje shadera
	Shader* shaderAlg2 = new Shader(argv[0], "algorithm2", "shader", "algorithm2");
	Shader* shaderAlg3 = new Shader(argv[0], "algorithm3", "shader", "algorithm3");
	Shader* shaderLine = new Shader(argv[0], "lineShader", "lineShader");

	// stvaranje mesha
	TriangleMesh* mesh = new TriangleMesh(argv[0], "glava\\glava.obj");
	mesh->normalize();

	// stvaranje objekta i povezivanje sa shaderom i meshom
	Object* object = new Object(shaderAlg2);
	object->addRenderable(mesh);
	object->setPosition(glm::vec3(0.5, 0.2, -2));
	object->scale(glm::vec3(0.5, 0.5, 0.5));
	object->globalMove(glm::vec3(-1, 0.1, 0));
	object->setOrientation(glm::vec3(1, -2, 1), glm::vec3(1, 1, 1));

	Object* object2 = new Object(shaderAlg3);
	object2->addRenderable(mesh);
	object2->scale(glm::vec3(0.2, 0.2, 0.2));
	object2->setPosition(glm::vec3(0.7, 1.6, -1.8));
	object2->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object2->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	// dodavanje bezierove krivulje
	Curve* bezier = new Curve(shaderLine);
	InputManager::bezierCurve = bezier; // postavlja krivulju koja ce se stvarati pritiskom na gumb

	// stvaranje renderera
	Renderer* renderer = new Renderer();
	renderer->registerRenderable(object);
	renderer->registerRenderable(object2);
	renderer->registerRenderable(bezier);

	// dodavanje na listu za prebacivanje kontrola
	InputManager::movable_candidates.push_back(object);
	InputManager::movable_candidates.push_back(object2);

	/*********************************************************************************************/
	//glavna petlja za prikaz
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		//pobrisi platno
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, InputManager::width, InputManager::height);

		// azuriranje animacije
		renderer->camera.update(deltaTime);

		// iscrtavanje objekta
		shaderAlg2->use();
		shaderAlg2->setUniform("eye", renderer->camera.getPosition());
		renderer->render();

		glfwSwapBuffers(window);
		InputManager::poll_events(window);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}   
		
	delete shaderAlg2;
	delete shaderAlg3;
	delete shaderLine;

	glfwTerminate();

    return EXIT_SUCCESS;
}
