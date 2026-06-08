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
#include <windows.h>

extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	/*********************************************************************************************/
	//postavljanje OpenGL konteksta, dohvacanje dostupnih OpenGL naredbi
	GLFWwindow* window;

	glfwInit();
	//gladLoadGL();

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	window = glfwCreateWindow(InputManager::width, InputManager::height, "Zadatak 9", nullptr, nullptr);
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
	std::cout << "" << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "" << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "" << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "" << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE); //ukljuci uklanjanje straznjih poligona -- za ovaj primjer je iskljuceno
	//glCullFace(GL_BACK); 

	glClearColor(0.15f, 0.1f, 0.1f, 1.0f); //boja brisanja platna izmedu iscrtavanja dva okvira
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // poligonsko iscrtavanje

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // zakljucaj i sakrij kursor
	glfwSwapInterval(1); //ne cekaj nakon iscrtavanja (vsync)
	FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak 9");

	// callback funkcije
	glfwSetScrollCallback(window, InputManager::scroll_callback);
	glfwSetCursorPosCallback(window, InputManager::mouse_callback);
	glfwSetKeyCallback(window, InputManager::key_callback);
	glfwSetFramebufferSizeCallback(window, InputManager::framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora

	// stvaranje shadera
	Shader* shaderAlg2 = new Shader(argv[0], "algorithm2", "shader", "algorithm2");
	//Shader* shaderAlg3 = new Shader(argv[0], "algorithm3", "shader", "algorithm3");
	Shader* shaderLine = new Shader(argv[0], "lineShader", "lineShader");
	Shader* shaderShadow = new Shader(argv[0], "algorithm3");

	// stvaranje mesha
	TriangleMesh* mesh = new TriangleMesh(argv[0], "glava\\glava.obj");
	TriangleMesh* floorMesh = new TriangleMesh(argv[0], "kocka.obj");
	mesh->normalize();

	// stvaranje objekta i povezivanje sa shaderom i meshom
	Object* object = new Object(shaderAlg2);
	object->addRenderable(mesh);
	object->setPosition(glm::vec3(0.5, 0.2, -2));
	object->scale(glm::vec3(0.5, 0.5, 0.5));
	object->globalMove(glm::vec3(-1, 0.1, 0));
	object->setOrientation(glm::vec3(1, -2, 1), glm::vec3(1, 1, 1));

	Object* object2 = new Object(shaderAlg2);
	object2->addRenderable(mesh);
	object2->scale(glm::vec3(0.2, 0.2, 0.2));
	object2->setPosition(glm::vec3(0.8, 0.2, -2));
	object2->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object2->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object3 = new Object(shaderAlg2);
	object3->addRenderable(mesh);
	object3->scale(glm::vec3(0.2, 0.2, 0.2));
	object3->setPosition(glm::vec3(0.5, 0.4, -1.2));
	object3->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object3->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object4 = new Object(shaderAlg2);
	object4->addRenderable(mesh);
	object4->scale(glm::vec3(0.2, 0.2, 0.2));
	object4->setPosition(glm::vec3(1, 0, -1.2));
	object4->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object4->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object5 = new Object(shaderAlg2);
	object5->addRenderable(mesh);
	object5->scale(glm::vec3(0.2, 0.2, 0.2));
	object5->setPosition(glm::vec3(0.8, -0.2, -1.5));
	object5->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 1, 0)));
	object5->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object6 = new Object(shaderAlg2);
	object6->addRenderable(mesh);
	object6->scale(glm::vec3(0.2, 0.2, 0.2));
	object6->setPosition(glm::vec3(0.7, 0.6, -1.8));
	object6->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object6->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object7 = new Object(shaderAlg2);
	object7->addRenderable(mesh);
	object7->scale(glm::vec3(0.2, 0.2, 0.2));
	object7->setPosition(glm::vec3(0.7, 1.6, -2.8));
	object7->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object7->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object8 = new Object(shaderAlg2);
	object8->addRenderable(mesh);
	object8->scale(glm::vec3(0.2, 0.2, 0.2));
	object8->setPosition(glm::vec3(0.7, 1.6, -0.8));
	object8->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object8->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object9 = new Object(shaderAlg2);
	object9->addRenderable(mesh);
	object9->scale(glm::vec3(0.2, 0.2, 0.2));
	object9->setPosition(glm::vec3(0.7, 0.6, -0.8));
	object9->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object9->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object10 = new Object(shaderAlg2);
	object10->addRenderable(mesh);
	object10->scale(glm::vec3(0.2, 0.2, 0.2));
	object10->setPosition(glm::vec3(0.7, 1.6, -1.8));
	object10->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object10->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object11 = new Object(shaderAlg2);
	object11->addRenderable(mesh);
	object11->scale(glm::vec3(0.2, 0.2, 0.2));
	object11->setPosition(glm::vec3(1.7, 0.6, 1));
	object11->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)));
	object11->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0)));

	Object* object12 = new Object(shaderAlg2);
	object12->addRenderable(mesh);
	object12->scale(glm::vec3(0.2, 0.2, 0.2));
	object12->setPosition(glm::vec3(-0.7, 0.6, 1));
	object12->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-25.0f), glm::vec3(0, 1, 0)));
	object12->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(1, 0, 0)));

	//floorMesh->normalize();
	Object* floor = new Object(shaderAlg2);
	floor->addRenderable(floorMesh);
	floor->scale(glm::vec3(10, 0.1, 10));
	floor->setPosition(glm::vec3(-5, -1, -5));

	// dodavanje bezierove krivulje
	Curve* bezier = new Curve(shaderLine);
	InputManager::bezierCurve = bezier; // postavlja krivulju koja ce se stvarati pritiskom na gumb

	// stvaranje renderera
	Renderer* renderer = new Renderer(shaderShadow);
	renderer->registerRenderable(object);
	renderer->registerRenderable(object2);
	renderer->registerRenderable(object3);
	renderer->registerRenderable(object4);
	renderer->registerRenderable(object5);
	renderer->registerRenderable(object6);
	renderer->registerRenderable(object7);
	renderer->registerRenderable(object8);
	renderer->registerRenderable(object9);
	renderer->registerRenderable(object10);
	renderer->registerRenderable(object11);
	renderer->registerRenderable(object12);
	renderer->registerRenderable(bezier);
	renderer->registerRenderable(floor);

	// dodavanje na listu za prebacivanje kontrola
	InputManager::movable_candidates.push_back(object);
	InputManager::movable_candidates.push_back(object2);
	InputManager::movable_candidates.push_back(object3);
	InputManager::movable_candidates.push_back(object4);
	InputManager::movable_candidates.push_back(object5);
	InputManager::movable_candidates.push_back(object6);
	InputManager::movable_candidates.push_back(object7);
	InputManager::movable_candidates.push_back(object8);

	// namjestanje svjetla
	Light* light = new Light(shaderLine, true);
	light->setPosition(glm::vec3(-1, 1, 2));
	light->mAmbientIntensity = glm::vec3(0.8, 0.8, 0.8);
	light->mIntensity = glm::vec3(1, 1, 1);
	renderer->mLights.push_back(light);

	Light* light2 = new Light(shaderLine, true);
	light2->setPosition(glm::vec3(2, 0.5, -3));
	light2->mIntensity = glm::vec3(0.5, 0.5, 0.5);
	renderer->mLights.push_back(light2);

	// reflektor
	Spotlight* spotlight = new Spotlight(shaderLine, true);
	spotlight->setPosition(glm::vec3(0.2, 3, 0));
	spotlight->setOrientation(glm::vec3(0, -3, -1), glm::vec3(0, 1, 0));
	spotlight->mIntensity = glm::vec3(1, 1, 1);
	spotlight->mCutoff = glm::radians(30.0);
	renderer->mLights.push_back(spotlight);
	/*Spotlight* spotlight2 = new Spotlight(shaderLine, true);
	spotlight2->setPosition(glm::vec3(-2, 1, 1));
	spotlight2->setOrientation(glm::vec3(1, 0, -1), glm::vec3(0, 1, 0));
	spotlight2->mIntensity = glm::vec3(1, 0, 0);
	spotlight2->mCutoff = glm::radians(30.0);
	renderer->mLights.push_back(spotlight2);
	Spotlight* spotlight3 = new Spotlight(shaderLine, true);
	spotlight3->setPosition(glm::vec3(2, 1, 0));
	spotlight3->setOrientation(glm::vec3(-1, -3, 0), glm::vec3(0, 1, 0));
	spotlight3->mIntensity = glm::vec3(0, 1, 0);
	spotlight3->mCutoff = glm::radians(30.0);
	renderer->mLights.push_back(spotlight3);*/

	renderer->initDepthmap();

	/*********************************************************************************************/
	//glavna petlja za prikaz
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		//pobrisi platno
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, InputManager::width, InputManager::height);

		// azuriranje animacija
		renderer->update(deltaTime);

		// iscrtavanje objekata
		renderer->render();

		glfwSwapBuffers(window);
		InputManager::poll_events(window);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

	delete renderer;
		
	delete shaderAlg2;
	//delete shaderAlg3;
	delete shaderLine;

	glfwTerminate();

    return EXIT_SUCCESS;
}
